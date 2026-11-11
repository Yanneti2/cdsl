#!/usr/bin/env Rscript
#
# Usage:
#   Rscript graph.r arquivo_jacobson.csv arquivo_bitvector.csv out.csv out.png operation_name [flags]
#
# Optional flags (from reference graph.r):
#   --min-run N                 ignore sign/winner changes that last fewer than
#                               N consecutive points (default: adaptive noise filter)
#   --interval-x a,b            fixed x range (also restricts fits / comparisons)
#   --interval-y a,b            fixed y range
#   --fit m1[,m2]               fit model(s) (linear, log, nlogn, power, constant)
#   --intersect                 use fitted curve intersection (uniroot) for transition
#   --no-log-x                  use linear x axis instead of log10
#   -t, --title TEXT            custom plot title
#   --y-label TEXT              custom y axis label

suppressPackageStartupMessages({
    library(ggplot2)
    library(tibble)
    library(dplyr)
    library(scales)
})


# ---------------------------------------------------------------- config ----

default_config <- function() {
    list(
        path0             = NULL,
        path1             = NULL,
        out_name_csv      = NULL,
        out_name_graph    = NULL,
        operation_name    = NULL,
        log_x             = TRUE,
        x_zero            = FALSE,
        y_zero            = FALSE,
        x_axis            = "Size",
        x_interval        = NULL,
        y_interval        = NULL,
        fit_models        = NULL,
        do_intersect      = FALSE,
        do_intersect_data = TRUE,
        min_run           = NULL,
        graph_title       = "Valor de x em função do Tamanho",
        y_label           = "Valor de x (escala pseudo-logarítmica)"
    )
}

FLAG_SPECS <- list(
    "--log-x"             = list(type = "switch",   keys = "log_x",             val = TRUE),
    "--no-log-x"          = list(type = "switch",   keys = "log_x",             val = FALSE),
    "--x-zero"            = list(type = "switch",   keys = "x_zero",            val = TRUE),
    "--y-zero"            = list(type = "switch",   keys = "y_zero",            val = TRUE),
    "--origin-zero"       = list(type = "switch",   keys = c("x_zero", "y_zero"), val = TRUE),
    "--intersect"         = list(type = "switch",   keys = "do_intersect",      val = TRUE),
    "--intersect-data"    = list(type = "switch",   keys = "do_intersect_data", val = TRUE),
    "--no-intersect-data" = list(type = "switch",   keys = "do_intersect_data", val = FALSE),
    "--output"            = list(type = "string",   keys = "out_name_graph"),
    "-o"                  = list(type = "string",   keys = "out_name_graph"),
    "--title"             = list(type = "string",   keys = "graph_title"),
    "-t"                  = list(type = "string",   keys = "graph_title"),
    "--x-axis"            = list(type = "string",   keys = "x_axis"),
    "--y-label"           = list(type = "string",   keys = "y_label"),
    "--interval-x"        = list(type = "interval", keys = "x_interval"),
    "--interval-y"        = list(type = "interval", keys = "y_interval"),
    "--fit"               = list(type = "csv",      keys = "fit_models"),
    "--min-run"           = list(type = "integer",  keys = "min_run")
)

parse_interval <- function(value, flag) {
    if (length(value) != 1 || is.na(value)) {
        stop(paste(flag, "requires two bounds, e.g. 0,100"))
    }
    value  <- gsub("^\\[|\\]$", "", value)
    bounds <- strsplit(value, ",", fixed = TRUE)[[1]]
    bounds <- suppressWarnings(as.numeric(trimws(bounds)))

    if (length(bounds) != 2 || any(!is.finite(bounds)) || bounds[1] >= bounds[2]) {
        stop(paste("Invalid interval for", flag,
                   "- expected two increasing numeric bounds, e.g. 0,100"))
    }
    bounds
}

parse_flag_value <- function(type, value, flag) {
    switch(type,
        string   = value,
        interval = parse_interval(value, flag),
        csv      = trimws(strsplit(value, ",", fixed = TRUE)[[1]]),
        integer  = {
            n <- suppressWarnings(as.integer(value))
            if (is.na(n) || n < 1) stop(paste(flag, "requires an integer >= 1"))
            n
        },
        stop(paste("Unknown flag type:", type))
    )
}

parse_args <- function(args) {
    config     <- default_config()
    positional <- character()
    index      <- 1

    while (index <= length(args)) {
        arg  <- args[index]
        spec <- FLAG_SPECS[[arg]]

        if (!is.null(spec)) {
            if (spec$type == "switch") {
                for (key in spec$keys) config[[key]] <- spec$val
            } else {
                if (index >= length(args)) stop(paste(arg, "requires a value"))
                index <- index + 1
                config[[spec$keys]] <- parse_flag_value(spec$type, args[index], arg)
            }
        } else {
            positional <- c(positional, arg)
        }
        index <- index + 1
    }

    if (length(positional) != 5) {
        stop("Erro: Forneça exatamente os cinco argumentos posicionais.\n",
             "Uso: Rscript graph.r arquivo_jacobson.csv arquivo_bitvector.csv out.csv out.png operacao [flags]")
    }

    config$path0          <- positional[1]
    config$path1          <- positional[2]
    config$out_name_csv   <- positional[3]
    config$out_name_graph <- positional[4]
    config$operation_name <- positional[5]

    if (config$do_intersect && is.null(config$fit_models)) {
        stop("--intersect compares fitted curves, so it needs --fit")
    }

    config
}


# ------------------------------------------------------------------ data ----

# Converts a column to numbers. Values that are not numbers become NA and are
# reported, so one bad row does not turn the whole column into text.
clean_numeric <- function(v, file, col) {
    if (is.numeric(v)) return(v)

    out <- suppressWarnings(as.numeric(trimws(as.character(v))))
    bad <- which(is.na(out))
    if (length(bad) > 0) {
        message(sprintf(
            "%s: column '%s' has %d non-numeric value(s), ignoring those rows (data row %s; first value: \"%s\")",
            file, col, length(bad), paste(head(bad, 5), collapse = ", "), v[bad[1]]))
    }
    out
}

load_data <- function(config) {
    df_0 <- read.csv(config$path0, sep = ";", header = TRUE, dec = ".")
    df_1 <- read.csv(config$path1, sep = ";", header = TRUE, dec = ".")

    rs_col <- if (config$operation_name == "jacobson" || config$operation_name == "rrr") {
        "RS_Time"
    } else {
        "BV_Time"
    }

    missing_0 <- setdiff(c("Size", "Time", rs_col), names(df_0))
    if (length(missing_0) > 0) {
        stop(sprintf("%s: missing column(s) %s (available: %s)",
                     config$path0, paste(missing_0, collapse = ", "),
                     paste(names(df_0), collapse = ", ")))
    }

    missing_1 <- setdiff(c("Size", "Time"), names(df_1))
    if (length(missing_1) > 0) {
        stop(sprintf("%s: missing column(s) %s (available: %s)",
                     config$path1, paste(missing_1, collapse = ", "),
                     paste(names(df_1), collapse = ", ")))
    }

    df_0$Size      <- clean_numeric(df_0$Size, config$path0, "Size")
    df_0$Time      <- clean_numeric(df_0$Time, config$path0, "Time")
    df_0[[rs_col]] <- clean_numeric(df_0[[rs_col]], config$path0, rs_col)
    df_0 <- df_0[is.finite(df_0$Size) & is.finite(df_0$Time) & is.finite(df_0[[rs_col]]), ]

    df_1$Size <- clean_numeric(df_1$Size, config$path1, "Size")
    df_1$Time <- clean_numeric(df_1$Time, config$path1, "Time")
    df_1 <- df_1[is.finite(df_1$Size) & is.finite(df_1$Time), ]

    # Agrupar os dados por Size
    if (config$operation_name == "jacobson" || config$operation_name == "rrr") {
        jac <- df_0 %>%
            group_by(Size) %>%
            summarise(
                Time_jac = mean(Time),
                RS_Time  = mean(RS_Time),
                .groups  = "drop"
            )
    } else {
        jac <- df_0 %>%
            group_by(Size) %>%
            summarise(
                Time_jac = mean(Time),
                RS_Time  = mean(BV_Time),
                .groups  = "drop"
            )
    }

    bv <- df_1 %>%
        group_by(Size) %>%
        summarise(
            Time_bv = mean(Time),
            .groups = "drop"
        )

    # Unir os dados agrupados e ordenar por Size
    dados_unidos <- inner_join(jac, bv, by = "Size") %>%
        arrange(Size) %>%
        mutate(
            x = RS_Time / (Time_bv - Time_jac)
        )

    if (!is.null(config$x_interval)) {
        dados_unidos <- dados_unidos %>%
            filter(Size >= config$x_interval[1], Size <= config$x_interval[2])
    }

    dados_unidos <- dados_unidos %>%
        filter(is.finite(Size), is.finite(x)) %>%
        mutate(
            Size_ant = lag(Size),
            x_ant    = lag(x)
        )

    # Guardar CSV apenas com Size e x
    dados_csv <- dados_unidos %>% select(Size, x)
    write.csv2(dados_csv, config$out_name_csv, row.names = FALSE)
    cat(paste("Ficheiro CSV gerado apenas com Size e x e guardado em:",
              config$out_name_csv, "\n"))

    dados_unidos
}


# ----------------------------------------------------------------- fits -----

fit_model <- function(x, y, model, xlim = NULL) {
    keep <- is.finite(x) & is.finite(y)
    if (model %in% c("log", "nlogn", "power")) keep <- keep & x > 0
    if (model == "power") keep <- keep & y > 0
    if (!is.null(xlim)) keep <- keep & x >= xlim[1] & x <= xlim[2]
    x <- x[keep]; y <- y[keep]

    m <- switch(model,
        linear   = lm(y ~ x),
        log      = lm(y ~ log(x)),
        nlogn    = lm(y ~ I(x * log(x))),
        power    = lm(log(y) ~ log(x)),
        constant = lm(y ~ 1),
        stop(paste("Unknown fit model:", model))
    )
    f <- function(xn) {
        p <- unname(predict(m, newdata = data.frame(x = xn)))
        if (model == "power") exp(p) else p
    }
    list(f = f, r2 = summary(m)$r.squared, model = model)
}


# ------------------------------------------------- "who is best" changes ----

# Given a matrix of y values (rows = increasing x, columns = candidates), finds
# the rows where the candidate with the LOWEST y changes.
#
# min_run > 1 ignores stretches where a candidate wins for fewer than min_run
# consecutive rows (they get absorbed into the stronger neighbouring stretch).
winner_changes <- function(y, min_run = 1) {
    y[!is.finite(y)] <- Inf
    winner <- apply(y, 1, which.min)

    runs <- rle(as.integer(winner))
    lens <- runs$lengths
    vals <- runs$values

    repeat {
        short <- which(lens < min_run)
        if (length(vals) < 2 || length(short) == 0) break

        i <- short[which.min(lens[short])]
        j <- if (i == 1) {
            2
        } else if (i == length(lens)) {
            i - 1
        } else if (lens[i + 1] > lens[i - 1]) {
            i + 1
        } else {
            i - 1
        }
        lens[j] <- lens[j] + lens[i]
        lens <- lens[-i]
        vals <- vals[-i]

        # neighbours with the same winner become one run
        group <- cumsum(c(TRUE, vals[-1] != vals[-length(vals)]))
        lens  <- as.vector(tapply(lens, group, sum))
        vals  <- vals[!duplicated(group)]
    }

    if (length(vals) < 2) {
        return(tibble(index = integer(), from = integer(), to = integer()))
    }

    starts <- cumsum(c(1, lens))[seq_along(lens)]
    tibble(index = starts[-1],
           from  = vals[-length(vals)],
           to    = vals[-1])
}

# Finds the clean sign transitions using winner_changes (with min_run noise
# filtering) so noisy oscillations where Time_bv ~ Time_jac do not produce
# dozens of overlapping rectangles and labels.
find_transitions <- function(dados_unidos, config) {
    if (nrow(dados_unidos) < 2) return(dados_unidos[0, ])

    min_run <- if (!is.null(config$min_run)) {
        config$min_run
    } else {
        max(5L, min(15L, ceiling(nrow(dados_unidos) * 0.05)))
    }

    # Column 1 wins when Time_bv < Time_jac (x < 0)
    # Column 2 wins when Time_jac < Time_bv (x > 0)
    y_mat <- cbind(dados_unidos$Time_bv, dados_unidos$Time_jac)
    ch    <- winner_changes(y_mat, min_run)

    if (nrow(ch) == 0 && is.null(config$min_run) && min_run > 2L) {
        ch <- winner_changes(y_mat, 2L)
    }

    if (nrow(ch) == 0) {
        return(dados_unidos[0, ] %>% mutate(size_mid = numeric()))
    }

    idx <- ch$index
    transicoes <- dados_unidos[idx, ] %>%
        mutate(
            Size_ant = dados_unidos$Size[pmax(1L, idx - 1L)],
            x_ant    = dados_unidos$x[pmax(1L, idx - 1L)],
            size_mid = sqrt(Size_ant * Size)
        )

    for (k in seq_len(nrow(transicoes))) {
        message(sprintf("Transition: Size %s -> %s (x = %.2f)",
                        transicoes$Size_ant[k], transicoes$Size[k], transicoes$x[k]))
    }

    transicoes
}


# -------------------------------------------------------------- plotting ----

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

pseudo_log_breaks <- function(limits) {
    max_abs <- max(abs(limits), na.rm = TRUE)
    if (!is.finite(max_abs) || max_abs <= 0) return(0)
    hi  <- ceiling(log10(max_abs))
    lo  <- if (max_abs > 200) 1 else 0
    pos <- 10^(lo:hi)
    all_b <- sort(unique(c(-rev(pos), 0, pos)))
    all_b[all_b >= limits[1] & all_b <= limits[2]]
}

# Builds a single summary label of all transitions placed in the top-left of
# the graph so the text box never covers the data points near the transition.
build_transition_labels <- function(transicoes, dados_unidos) {
    if (nrow(transicoes) == 0) return(transicoes)

    transicoes <- transicoes %>%
        arrange(size_mid) %>%
        mutate(single_label = paste0("Size: ", Size_ant, " -> ", Size))

    x_left <- min(dados_unidos$Size, na.rm = TRUE)
    y_top  <- max(c(dados_unidos$x, abs(dados_unidos$x), 10), na.rm = TRUE)

    tibble(
        x_pos = x_left,
        y_pos = y_top,
        label = paste(transicoes$single_label, collapse = "\n")
    )
}

build_plot <- function(dados_unidos, transicoes, config) {
    grafico_x <- ggplot(dados_unidos, aes(x = Size, y = x)) +
        geom_hline(yintercept = 0, linetype = "dotted", color = "grey40") +
        # Linha dos valores
        geom_line(color = "#2b5c8f", linewidth = 0.8) +
        # Pontos
        geom_point(color = "#1f4068", size = 1.5)

    if (nrow(transicoes) > 0) {
        lbl_df <- build_transition_labels(transicoes, dados_unidos)
        grafico_x <- grafico_x +
            # IDENTIFICAÇÃO DO INTERVALO (canto superior esquerdo)
            geom_label(
                data = lbl_df,
                aes(
                    x     = x_pos,
                    y     = y_pos,
                    label = label
                ),
                inherit.aes = FALSE,
                color    = "red",
                fill     = "white",
                fontface = "bold",
                size     = 3.5,
                hjust    = 0,
                vjust    = 1
            )
    }

    # Escala X com marcações limpas (evita sobreposição quando há múltiplas transições)
    x_breaks_fn <- function(lim) {
        base_b <- scales::breaks_log(n = 10)(lim)
        base_b <- base_b[is.finite(base_b) & base_b >= lim[1] & base_b <= lim[2]]

        if (nrow(transicoes) > 0 && nrow(transicoes) <= 4) {
            # Greedily select transition ticks that are at least 0.08 log10 units apart
            t_cands <- sort(unique(c(transicoes$Size_ant, transicoes$Size)))
            t_cands <- t_cands[is.finite(t_cands) & t_cands > 0]
            t_kept  <- numeric()
            for (tv in t_cands) {
                if (length(t_kept) == 0 || min(abs(log10(t_kept) - log10(tv))) >= 0.08) {
                    t_kept <- c(t_kept, tv)
                }
            }
            # Keep base breaks that do not collide with any kept transition tick
            base_keep <- vapply(base_b, function(b) {
                length(t_kept) == 0 || min(abs(log10(t_kept) - log10(b))) >= 0.08
            }, logical(1))
            sort(unique(c(base_b[base_keep], t_kept)))
        } else {
            base_b
        }
    }

    grafico_x <- grafico_x +
        scale_x_log10(
            limits       = config$x_interval,
            breaks       = x_breaks_fn,
            minor_breaks = log_minor_breaks,
            labels       = scales::label_number(
                big.mark     = ".",
                decimal.mark = ","
            )
        ) +
        # ESCALA Y PARA NEGATIVOS E POSITIVOS
        scale_y_continuous(
            trans  = scales::pseudo_log_trans(sigma = 1),
            limits = config$y_interval,
            breaks = pseudo_log_breaks,
            labels = scales::label_number(
                big.mark     = ".",
                decimal.mark = ","
            )
        ) +
        labs(
            title    = config$graph_title,
            # subtitle = "Transições de sinal destacadas em vermelho",
            x        = "Size (escala logarítmica)",
            y        = config$y_label
        ) +
        theme_minimal(base_size = 13) +
        theme(
            plot.title    = element_text(face = "bold", hjust = 0.5),
            plot.subtitle = element_text(hjust = 0.5),
            axis.text.x   = element_text(angle = 45, hjust = 1)
        )

    grafico_x
}


# ------------------------------------------------------------------ main ----

main <- function(args) {
    config       <- parse_args(args)
    dados_unidos <- load_data(config)
    transicoes   <- find_transitions(dados_unidos, config)
    grafico_x    <- build_plot(dados_unidos, transicoes, config)

    ggsave(config$out_name_graph, plot = grafico_x, width = 9, height = 5, dpi = 300)
    cat(paste("Gráfico gerado com sucesso e guardado em:", config$out_name_graph, "\n"))
}

if (!interactive()) main(commandArgs(trailingOnly = TRUE))
