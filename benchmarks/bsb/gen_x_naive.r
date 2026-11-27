#!/usr/bin/env Rscript

suppressPackageStartupMessages({
    library(dplyr)
})

data_dir <- "data"
out_dir  <- "x_naive"

densities  <- c(".1", "1", "5", "10", "30", "50", "70", "90", "95", "99", "99.9")
operations <- c("r1", "r0", "s1", "s0")

# Structure config: output folder/prefix -> data folder and CSV prefix
structures <- list(
    bitvectorBS = list(dir = "bitvectorBS",  prefix = "bsb-bitvectorBS-1-10k"),
    jacobson    = list(dir = "jacobsonrank", prefix = "bsb-jacobsonrank-1-10k"),
    rrr         = list(dir = "rrr",          prefix = "bsb-rrr-1-10k")
)

build_col_for <- function(struct, op) {
    if (struct == "bitvectorBS") {
        "BV_Time"
    } else if (struct == "jacobson") {
        if (startsWith(op, "r")) "RS_Time" else "SS_Time"
    } else {
        "RS_Time"
    }
}

read_grouped <- function(path, cols) {
    df <- read.csv(path, sep = ";", header = TRUE, dec = ".", check.names = FALSE)
    for (col in c("Size", cols)) {
        df[[col]] <- suppressWarnings(as.numeric(df[[col]]))
    }
    df <- df[complete.cases(df[, c("Size", cols), drop = FALSE]), , drop = FALSE]
    df %>%
        group_by(Size) %>%
        summarise(across(all_of(cols), ~ mean(.x, na.rm = TRUE)), .groups = "drop")
}

generated <- 0L

for (s in names(structures)) {
    dir.create(file.path(out_dir, s), recursive = TRUE, showWarnings = FALSE)
    cfg <- structures[[s]]

    for (op in operations) {
        bcol <- build_col_for(s, op)

        for (d in densities) {
            struct_file <- file.path(data_dir, cfg$dir, sprintf("%s-%s-d%s.csv", cfg$prefix, op, d))
            naive_file  <- file.path(data_dir, "bitvector", sprintf("bsb-bitvector-1-10k-%s-d%s.csv", op, d))

            if (!file.exists(struct_file) || !file.exists(naive_file)) {
                next
            }

            st <- read_grouped(struct_file, c("Time", bcol)) %>%
                rename(Time_struct = Time, Build_time = !!sym(bcol))
            nv <- read_grouped(naive_file, "Time") %>%
                rename(Time_naive = Time)

            res <- inner_join(st, nv, by = "Size") %>%
                arrange(Size) %>%
                mutate(x = Build_time / (Time_naive - Time_struct)) %>%
                filter(is.finite(Size), is.finite(x)) %>%
                select(Size, x)

            out_file <- file.path(out_dir, s, sprintf("%s-%s-d%s.csv", s, op, d))
            write.table(res, out_file, sep = ";", row.names = FALSE, col.names = TRUE)
            generated <- generated + 1L
        }
    }
}

cat(sprintf("Generated %d break-even CSV files in %s/\n", generated, out_dir))
