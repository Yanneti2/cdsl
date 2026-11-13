library(dplyr)

args <- commandArgs(trailingOnly = TRUE)
print(args)

output_file <- "output.csv"
input_file <- ""

group_by <- "Size"
group <- "Time"

args_index <- 1
while (args_index <= length(args)) {
    arg <- args[args_index]
    if (arg == "-o") {
        args_index <- args_index + 1
        if (args_index > length(args)) {
            error("Missing output file")
        }
        output_file <- args[args_index]
    } else if (arg == "--group") {
        args_index <- args_index + 1
        if (args_index > length(args)) {
            error("Missing group parameter")
        }
        group <- args[args_index]
    } else if (arg == "--group-by") {
        args_index <- args_index + 1
        if (args_index > length(args)) {
            error("Missing group by parameter")
        }
        group_by <- args[args_index]
    } else {
        input_file <- args[args_index]
    }

    args_index <- args_index + 1
}

if (input_file == "") {
    error("Missing input file")
}

data <- read.csv(input_file, sep = ";")
data$x <- as.numeric(as.character(data$x))

data <- data %>%
  group_by(.data[[group_by]]) %>%
  filter(
    .data[[group]] >= quantile(.data[[group]], 0.25) - 1.5 * IQR(.data[[group]]) &
    .data[[group]] <= quantile(.data[[group]], 0.75) + 1.5 * IQR(.data[[group]])
  ) %>%
  summarise(
    "{group}" := mean(.data[[group]])
  )

write.table(data, file = output_file, row.names = FALSE, sep = ";")
