args <- commandArgs(trailingOnly=TRUE)

# Defining the parameters
log_x <- FALSE
log_y <- FALSE
x_zero <- FALSE
y_zero <- FALSE
datasets <- c()
output_file <- "result.png"
graph_title <- NULL
x_axis <- "Size"
y_axis <- "Time"

# Parsing arguments
if (length(args) == 0){
    stop("No input file provided")
}

index <- 1
n_datasets <- 0
dataset_names <- c()
is_prev_dataset <- FALSE
is_flag <- FALSE

flags <- c()
flags <- c(flags,
           "--log-x",
           "--log-y",
           "--x-zero",
           "--y-zero",
           "--origin-zero",
           "-o",
           "--output",
           "--title",
           "-t",
           "--x-axis",
           "--y-axis"
)

while(index <= length(args)) {
    arg <- args[index]
    if (arg %in% flags)
    {
        if (is_prev_dataset)
        {
            stop("You need to set a name to your dataset\n
                 format: graph.r {Dataset.csv} {Title} [flags]")
        }

        if (arg == "--log-x")
        {
            log_x <- TRUE
        }
        else if (arg == "--log-y")
        {
            log_y <- TRUE
        }
        else if (arg == "--x-zero")
        {
            x_zero <- TRUE
        }
        else if (arg == "--y-zero")
        {
            y_zero <- TRUE
        }
        else if (arg == "--origin-zero")
        {
            x_zero <- TRUE
            y_zero <- TRUE
        }
        else if (arg == "--output" | arg == "-o")
        {
            output_file <- args[index + 1]
            index <- index + 1
        }
        else if (arg == "--title" | arg == "-t")
        {
            graph_title <- args[index + 1]
            index <- index + 1
        }
        else if (arg == "--x-axis")
        {
            x_axis <- args[index + 1]
            index <- index + 1
        }
        else if (arg == "--y-axis")
        {
            y_axis <- args[index + 1]
            index <- index + 1
        }


        is_prev_dataset <- FALSE
        index <- index + 1
        next
    }


    if (!is_prev_dataset)
    {
        n_datasets <- n_datasets + 1
        datasets <- c(datasets, arg)
        index <- index + 1
        is_prev_dataset <- TRUE
        next
    }

    dataset_names <- c(dataset_names, arg)
    is_prev_dataset <- FALSE



    index <- index + 1
    
}


if (n_datasets == 0)
{
    stop("No datasets provided")
}


# Loading ggplot
library(ggplot2)
library(tibble)
library(dplyr)

data <- bind_rows(
    lapply(seq_len(n_datasets), function(i) {
        df <- read.csv(datasets[i], sep = ";")
        df$Operation <- dataset_names[i]
        df
    })
)

df <- tibble(data)


log_minor_breaks <- function(limits) {
    lo <- floor(log10(limits[1]))
    hi <- ceiling(log10(limits[2]))
    as.vector(outer(1:9, 10^(lo:hi)))
}

log_major_breaks <- function(limits) {
    lo <- ceiling(log10(limits[1]))
    hi <- floor(log10(limits[2]))
    10^(lo:hi)
}



# Plotting the graph
plt <- ggplot(df, aes(
      x=.data[[x_axis]],
      y=.data[[y_axis]],
      color=Operation,
      shape=Operation)
) + geom_point()



# Applying the settings
plt <- plt +
    scale_x_continuous(
        trans = if (log_x) "log10" else "identity",
        limits = if (x_zero) {
            c(if (log_x) 1 else 0, NA)
        } else {
            c(NA, NA)
        },
        expand = if (x_zero) expansion(mult = c(0, 0.05)) else waiver(),
        breaks = if (log_x) log_major_breaks else waiver(),
        minor_breaks = if (log_x) log_minor_breaks else waiver(),
        labels = if (log_x) scales::label_log() else waiver()
    ) +
    scale_y_continuous(
        trans = if (log_y) "log10" else "identity",
        limits = if (y_zero) {
            c(if (log_y) 1 else 0, NA)
        } else {
            c(NA, NA)
        },
        expand = if (y_zero) expansion(mult = c(0, 0.05)) else waiver(),
        breaks = if (log_y) log_major_breaks else waiver(),
        minor_breaks = if (log_y) log_minor_breaks else waiver(),
        labels = if (log_y) scales::label_log() else waiver()
    ) +
    labs(
         title = graph_title,
    )


ggsave(
       output_file,
       plt,
       device="png"
)
