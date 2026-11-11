#!/usr/bin/env Rscript
#
# compare_structures.R - Script de comparação de Estruturas de Dados Compactas
# Baseado no graph.r, com suporte a cálculo automático de métricas derivadas
# e normalização em relação ao BitVector (Baseline).
#
# Uso:
#   Rscript compare_structures.R {Dataset.csv} {Nome} [{Dataset.csv} {Nome} ...] [flags]
#
# Exemplos:
#   Rscript compare_structures.R \
#     bitvector-bsb-r0-d10.csv BitVector \
#     bitvectorBS-bsb-r0-d10.csv BitVectorBS \
#     bsb-rrr-r0-d10.csv RRR \
#     jacobson-bsb-r0-d10.csv Jacobson \
#     --metric Op_Time --bars normalize --baseline BitVector -o time_norm.png
#

suppressPackageStartupMessages({
    library(ggplot2)
    library(tibble)
    library(dplyr)
})

# ---------------------------------------------------------------- config ----

default_config <- function() {
    list(
        datasets          = character(),
        dataset_names     = character(),
        metric            = "Op_Time", # Op_Time, Const_Time, Total_Mem, Speedup, Mem_Ratio
        log_x             = FALSE,
        log_y             = FALSE,
        x_zero            = FALSE,
        y_zero            = FALSE,
        output_file       = "result.png",
        graph_title       = NULL,
        x_axis            = "Size",
        x_interval        = NULL,
        y_interval        = NULL,
        fit_models        = NULL,
        do_intersect      = FALSE,
        do_intersect_data = FALSE,
        min_run           = 1L,
        bars              = NULL,
        baseline          = "BitVector",
        y_label           = NULL,
        legend_title      = "Estrutura",
        x_values          = NULL,
        summary_table     = TRUE
    )
}

Y_COL <- "Y_VALUE"
BAR_MODES <- c("linear", "log", "normalize")

FLAG_SPECS <- list(
    "--metric"         = list(type = "string",   keys = "metric"),
    "--log-x"          = list(type = "switch",   keys = "log_x"),
    "--log-y"          = list(type = "switch",   keys = "log_y"),
    "--x-zero"         = list(type = "switch",   keys = "x_zero"),
    "--y-zero"         = list(type = "switch",   keys = "y_zero"),
    "--origin-zero"    = list(type = "switch",   keys = c("x_zero", "y_zero")),
    "--intersect"      = list(type = "switch",   keys = "do_intersect"),
    "--intersect-data" = list(type = "switch",   keys = "do_intersect_data"),
    "--output"         = list(type = "string",   keys = "output_file"),
    "-o"               = list(type = "string",   keys = "output_file"),
    "--title"          = list(type = "string",   keys = "graph_title"),
    "-t"               = list(type = "string",   keys = "graph_title"),
    "--x-axis"         = list(type = "string",   keys = "x_axis"),
    "--y-label"        = list(type = "string",   keys = "y_label"),
    "--legend"         = list(type = "string",   keys = "legend_title"),
    "--x-values"       = list(type = "numbers",  keys = "x_values"),
    "--interval-x"     = list(type = "interval", keys = "x_interval"),
    "--interval-y"     = list(type = "interval", keys = "y_interval"),
    "--fit"            = list(type = "csv",      keys = "fit_models"),
    "--min-run"        = list(type = "integer",  keys = "min_run"),
    "--bars"           = list(type = "string",   keys = "bars"),
    "--baseline"       = list(type = "string",   keys = "baseline")
)

parse_interval <- function(value, flag) {
    if (length(value) != 1 || is.na(value)) stop(paste(flag, "requer dois limites, ex: 0,100"))
    value <- gsub("^\\[|\\]$", "", value)
    bounds <- suppressWarnings(as.numeric(trimws(strsplit(value, ",", fixed = TRUE)[[1]])))
    if (length(bounds) != 2 || any(!is.finite(bounds)) || bounds[1] >= bounds[2]) {
        stop(paste("Intervalo inválido para", flag))
    }
    bounds
}

parse_flag_value <- function(type, value, flag) {
    switch(type,
        string   = value,
        interval = parse_interval(value, flag),
        csv      = trimws(strsplit(value, ",", fixed = TRUE)[[1]]),
        numbers  = suppressWarnings(as.numeric(trimws(strsplit(value, ",", fixed = TRUE)[[1]]))),
        integer  = suppressWarnings(as.integer(value)),
        stop(paste("Tipo de flag desconhecido:", type))
    )
}

parse_args <- function(args) {
    if (length(args) == 0) stop("Nenhum arquivo de entrada fornecido.")
    config <- default_config()
    index  <- 1

    while (index <= length(args)) {
        arg  <- args[index]
        spec <- FLAG_SPECS[[arg]]

        if (!is.null(spec)) {
            if (spec$type == "switch") {
                for (key in spec$keys) config[[key]] <- TRUE
            } else {
                if (index >= length(args)) stop(paste(arg, "requer um valor"))
                index <- index + 1
                config[[spec$keys]] <- parse_flag_value(spec$type, args[index], arg)
            }
        } else if (length(config$datasets) == length(config$dataset_names)) {
            config$datasets <- c(config$datasets, arg)
        } else {
            config$dataset_names <- c(config$dataset_names, arg)
        }
        index <- index + 1
    }

    if (length(config$datasets) == 0) stop("Forneça os arquivos .csv")
    if (length(config$datasets) != length(config$dataset_names)) {
        stop("Cada dataset CSV deve ter um nome de identificação associado.")
    }

    if (is.null(config$y_label)) {
        config$y_label <- switch(config$metric,
            "Op_Time"    = "Tempo por Operação (ns/op)",
            "Const_Time" = "Tempo de Construção Total",
            "Total_Mem"  = "Memória Total (Bytes)",
            "Speedup"    = "Speedup vs Baseline (BitVector)",
            "Mem_Ratio"  = "Razão de Memória vs Baseline",
            config$metric
        )
    }

    config
}

# ------------------------------------------------------------- data loading ----

# Processa colunas heterogêneas dos CSVs e calcula métricas unificadas
process_csv_data <- function(filepath, dataset_name, x_axis_name) {
    raw <- read.csv(filepath, sep = ";")

    if (!x_axis_name %in% names(raw)) {
        stop(sprintf("%s: coluna x '%s' não encontrada.", filepath, x_axis_name))
    }

    x_vals <- as.numeric(raw[[x_axis_name]])

    # 1. Tempo por Operação
    op_time <- if ("Time" %in% names(raw)) as.numeric(raw[["Time"]]) else NA

    # 2. Tempo de Construção Total (Soma de BV_Time, RS_Time, SS_Time, ignorando -1)
    const_cols <- intersect(c("BV_Time", "RS_Time", "SS_Time"), names(raw))
    const_time <- if (length(const_cols) > 0) {
        rowSums(sapply(raw[const_cols], function(col) {
            val <- as.numeric(col)
            ifelse(is.na(val) | val == -1, 0, val)
        }))
    } else {
        rep(0, nrow(raw))
    }

    # 3. Memória Total (Soma de BV_Memory, RS_Memory, SS_Memory, ignorando -1)
    mem_cols <- intersect(c("BV_Memory", "RS_Memory", "SS_Memory"), names(raw))
    total_mem <- if (length(mem_cols) > 0) {
        rowSums(sapply(raw[mem_cols], function(col) {
            val <- as.numeric(col)
            ifelse(is.na(val) | val == -1, 0, val)
        }))
    } else {
        rep(0, nrow(raw))
    }

    data.frame(
        Size       = x_vals,
        Op_Time    = op_time,
        Const_Time = const_time,
        Total_Mem  = total_mem,
        Operation  = dataset_name,
        stringsAsFactors = FALSE
    )
}

load_all_data <- function(config) {
    frames <- lapply(seq_along(config$datasets), function(i) {
        process_csv_data(config$datasets[i], config$dataset_names[i], config$x_axis)
    })
    df <- bind_rows(frames)

    # Cálculo de métricas relativas ao baseline (BitVector)
    if (config$baseline %in% config$dataset_names) {
        base_df <- df %>%
            filter(Operation == config$baseline) %>%
            group_by(Size) %>%
            summarise(
                Base_Op_Time   = mean(Op_Time, na.rm = TRUE),
                Base_Total_Mem = mean(Total_Mem, na.rm = TRUE),
                .groups = "drop"
            )

        df <- df %>%
            left_join(base_df, by = "Size") %>%
            mutate(
                Speedup   = Base_Op_Time / Op_Time,
                Mem_Ratio = Total_Mem / Base_Total_Mem
            )
    }

    df[[Y_COL]] <- df[[config$metric]]
    df <- df[is.finite(df[[config$x_axis]]) & is.finite(df[[Y_COL]]), ]
    tibble(df)
}

# ----------------------------------------------------------- summary report ----

print_summary_report <- function(df, config) {
    cat("\n=== RESUMO COMPARATIVO DE ESTRUTURAS DE DADOS ===\n")
    cat(sprintf("Baseline selecionado: %s\n\n", config$baseline))

    summary_stats <- df %>%
        group_by(Operation) %>%
        summarise(
            `Op Time Médio`    = round(mean(Op_Time, na.rm = TRUE), 2),
            `Const Time Médio` = round(mean(Const_Time, na.rm = TRUE), 2),
            `Memória Média`    = round(mean(Total_Mem, na.rm = TRUE), 2),
            `Speedup Médio`    = if ("Speedup" %in% names(.)) round(mean(Speedup, na.rm = TRUE), 2) else NA,
            .groups = "drop"
        )

    print(as.data.frame(summary_stats), row.names = FALSE)
    cat("\n===================================================\n\n")
}

# ------------------------------------------------------------- plotting --------

axis_scale <- function(scale_fn, use_log, zero, interval) {
    scale_fn(
        trans = if (use_log) "log10" else "identity",
        limits = if (!is.null(interval)) interval else if (zero) c(if (use_log) 1 else 0, NA) else c(NA, NA),
        expand = if (!is.null(interval)) expansion(mult = 0) else if (zero) expansion(mult = c(0, 0.05)) else waiver()
    )
}

line_plot <- function(df, config) {
    plt <- ggplot(df, aes(x = Size, y = .data[[Y_COL]], color = Operation, shape = Operation)) +
        geom_line(alpha = 0.6) +
        geom_point(size = 1.5) +
        axis_scale(scale_x_continuous, config$log_x, config$x_zero, config$x_interval) +
        axis_scale(scale_y_continuous, config$log_y, config$y_zero, config$y_interval) +
        labs(
            title = ifelse(is.null(config$graph_title), paste("Comparação de", config$y_label), config$graph_title),
            x     = config$x_axis,
            y     = config$y_label,
            color = config$legend_title,
            shape = config$legend_title
        ) +
        theme_minimal(base_size = 12)
    plt
}

bar_plot <- function(df, config) {
    d <- df %>%
        group_by(Operation, Size) %>%
        summarise(y = mean(.data[[Y_COL]], na.rm = TRUE), .groups = "drop")

    if (!is.null(config$x_values)) {
        d <- d %>% filter(Size %in% config$x_values)
    }

    if (config$bars == "normalize") {
        base_d <- d %>% filter(Operation == config$baseline) %>% select(Size, base_y = y)
        d <- d %>%
            inner_join(base_d, by = "Size") %>%
            mutate(
                y = y / base_y,
                label = paste0(round(y, 2), "x")
            )
    }

    d$Size_Fact <- factor(d$Size, levels = sort(unique(d$Size)))
    dodge <- position_dodge(width = 0.9)

    plt <- ggplot(d, aes(x = Size_Fact, y = y, fill = Operation)) +
        geom_col(position = dodge, width = 0.8) +
        labs(
            title = ifelse(is.null(config$graph_title), paste("Comparação em Barras -", config$y_label), config$graph_title),
            x     = config$x_axis,
            y     = if (config$bars == "normalize") paste(config$y_label, "(Relativo a", config$baseline, ")") else config$y_label,
            fill  = config$legend_title
        ) +
        theme_minimal(base_size = 12)

    if (config$bars == "normalize") {
        plt <- plt +
            geom_hline(yintercept = 1, linetype = "dashed", color = "grey40") +
            geom_text(aes(label = label), position = dodge, vjust = -0.4, size = 3)
    }
    plt
}

# ------------------------------------------------------------------ main ----

main <- function(args) {
    config <- parse_args(args)
    df <- load_all_data(config)

    if (config$summary_table) {
        print_summary_report(df, config)
    }

    plt <- if (is.null(config$bars)) line_plot(df, config) else bar_plot(df, config)

    ggsave(config$output_file, plt, width = 9, height = 5.5, dpi = 300)
    cat(sprintf("Gráfico salvo com sucesso em: %s\n", config$output_file))
}

if (!interactive()) {
    main(commandArgs(trailingOnly = TRUE))
}