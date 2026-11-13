library(dplyr)

args <- commandArgs(trailingOnly = TRUE)

output_file <- "output.csv"
input_file <- ""

group_by <- "Size"

args_index <- 1
while (args_index <= length(args)) {
    arg <- args[args_index]
    if (arg == "-o") {
        args_index <- args_index + 1
        if (args_index > length(args)) {
            error("Missing output file")
        }
        output_file <- args[args_index]
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
data <- data %>%
  mutate_all(as.numeric)

value_cols <- data %>% 
  select(-all_of(group_by)) %>% 
  names()

data <- data %>%
  group_by(.data[[group_by]]) %>%
  mutate(
    across(
      all_of(value_cols),
      ~ {
        Q1 <- quantile(.x, 0.25, na.rm = TRUE)
        Q3 <- quantile(.x, 0.75, na.rm = TRUE)
        IQR <- Q3 - Q1
        
        ifelse(.x >= (Q1 - 1.5 * IQR) & .x <= (Q3 + 1.5 * IQR), .x, NA)
      }
    )
  ) %>% 
  summarise(
    across(all_of(value_cols), ~ mean(.x, na.rm = TRUE)),
    .groups = "drop"
  )

write.table(data, file = output_file, row.names = FALSE, sep = ";")
