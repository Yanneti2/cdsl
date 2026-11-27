#!/usr/bin/env Rscript

suppressPackageStartupMessages({
    library(dplyr)
})

data_dir <- "data"
out_dir  <- "memory_overhead"

dir.create(out_dir, recursive = TRUE, showWarnings = FALSE)

densities  <- c(".1", "1", "5", "10", "30", "50", "70", "90", "95", "99", "99.9")
operations <- c("r1", "r0", "s1", "s0")

# Structure config: label used by table_graph.r/heatmap.r -> data folder and CSV prefix
structures <- list(
    bs       = list(dir = "bitvectorBS",  prefix = "bsb-bitvectorBS-1-10k"),
    jacobson = list(dir = "jacobsonrank", prefix = "bsb-jacobsonrank-1-10k"),
    rrr      = list(dir = "rrr",          prefix = "bsb-rrr-1-10k")
)

read_total_mem <- function(path, struct, op) {
    df <- read.csv(path, sep = ";", header = TRUE, dec = ".", check.names = FALSE)
    df$Size <- suppressWarnings(as.numeric(df$Size))

    if (struct == "naive" || struct == "bs") {
        df$Mem <- suppressWarnings(as.numeric(df$BV_Memory))
    } else if (struct == "rrr") {
        df$Mem <- suppressWarnings(as.numeric(df$RS_Memory))
    } else if (struct == "jacobson") {
        bv <- suppressWarnings(as.numeric(df$BV_Memory))
        rs <- suppressWarnings(as.numeric(df$RS_Memory))
        if (startsWith(op, "r")) {
            df$Mem <- bv + rs
        } else {
            ss <- suppressWarnings(as.numeric(df$SS_Memory))
            df$Mem <- bv + rs + ss
        }
    }

    df <- df[is.finite(df$Size) & is.finite(df$Mem), c("Size", "Mem"), drop = FALSE]
    df %>%
        group_by(Size) %>%
        summarise(Mem = mean(Mem, na.rm = TRUE), .groups = "drop")
}

for (op in operations) {
    rows <- list()

    for (d in densities) {
        naive_file <- file.path(data_dir, "bitvector",
                                sprintf("bsb-bitvector-1-10k-%s-d%s.csv", op, d))
        if (!file.exists(naive_file)) next

        nv <- read_total_mem(naive_file, "naive", op) %>%
            rename(Mem_naive = Mem) %>%
            filter(Mem_naive > 0)

        dens_num <- as.numeric(d)

        for (s in names(structures)) {
            cfg <- structures[[s]]
            struct_file <- file.path(data_dir, cfg$dir,
                                     sprintf("%s-%s-d%s.csv", cfg$prefix, op, d))
            if (!file.exists(struct_file)) next

            st <- read_total_mem(struct_file, s, op) %>%
                rename(Mem_struct = Mem)

            joined <- inner_join(st, nv, by = "Size") %>%
                arrange(Size) %>%
                transmute(
                    Structure = s,
                    Size      = Size,
                    Density   = dens_num,
                    Overhead  = (Mem_struct - Mem_naive) / Mem_naive * 100
                ) %>%
                filter(is.finite(Overhead))

            rows[[length(rows) + 1L]] <- joined
        }
    }

    out_df <- bind_rows(rows)
    out_file <- file.path(out_dir, sprintf("memory-%s.csv", op))
    write.table(out_df, out_file, sep = ";", row.names = FALSE, col.names = TRUE, quote = FALSE)
    cat(sprintf("Generated %s (%d rows)\n", out_file, nrow(out_df)))
}
