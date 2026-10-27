#!/usr/bin/env Rscript
#
# Usage:
#   graph.r {Dataset.csv} {Name} [{Dataset.csv} {Name} ...] [flags]
#
# Flags:
#   --log-x / --log-y           log10 axes
#   --x-zero / --y-zero         start the axis at 0 (1 on log axes)
#   --origin-zero               both of the above
#   --interval-x a,b            fixed x range (also restricts fits / comparisons)
#   --interval-y a,b            fixed y range
#   --x-axis NAME               x column (default: Size)
#   --y-axis NAME               y column (default: Time)
#   -t, --title TEXT            plot title
#   -o, --output FILE           output file (default: result.png)
#   --fit m1,m2,...             fit one model per dataset (or one for all):
#                               linear, log, nlogn, power, constant
#   --intersect                 highlight where the best (lowest) FIT changes
#   --intersect-data            highlight where the best (lowest) DATA POINT changes
#   --min-run N                 with --intersect-data: ignore winner changes that
#                               last fewer than N consecutive points (noise filter)
#   --bars MODE                 grouped bar chart instead of points/lines: one
#                               group per x value, one bar per dataset.
#                               MODE: linear | log | normalize
#                                 linear     bars start at 0
#                                 log        log10 y axis (bars start at 1, or at
#                                            the lower bound of --interval-y)
#                                 normalize  every bar divided by the baseline
#                                            dataset at the same x, labelled "5.9x"
#   --baseline NAME             dataset used as 1x by --bars normalize
#                               (default: the first dataset)


# ---------------------------------------------------------------- config ----

default_config <- function() {
    list(
        datasets          = character(),
        dataset_names     = character(),
        log_x             = FALSE,
        log_y             = FALSE,
        x_zero            = FALSE,
        y_zero            = FALSE,
        output_file       = "result.png",
        graph_title       = NULL,
        x_axis            = "Size",
        y_axis            = "Time",
        x_interval        = NULL,
        y_interval        = NULL,
        fit_models        = NULL,
        do_intersect      = FALSE,
        do_intersect_data = FALSE,
        min_run           = 1L,
        bars              = NULL,
        baseline          = NULL
    )
}

# flag -> list(type, config keys it sets)
FLAG_SPECS <- list(
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
    "--y-axis"         = list(type = "string",   keys = "y_axis"),
    "--interval-x"     = list(type = "interval", keys = "x_interval"),
    "--interval-y"     = list(type = "interval", keys = "y_interval"),
    "--fit"            = list(type = "csv",      keys = "fit_models"),
    "--min-run"        = list(type = "integer",  keys = "min_run"),
    "--bars"           = list(type = "string",   keys = "bars"),
    "--baseline"       = list(type = "string",   keys = "baseline")
)

BAR_MODES <- c("linear", "log", "normalize")

parse_interval <- function(value, flag) {
    if (length(value) != 1 || is.na(value)) {
        stop(paste(flag, "requires two bounds, e.g. 0,100"))
    }

    # Allow optional surrounding brackets: [0,100]
    value <- gsub("^\\[|\\]$", "", value)

    bounds <- strsplit(value, ",", fixed = TRUE)[[1]]
    bounds <- suppressWarnings(as.numeric(trimws(bounds)))

    if (length(bounds) != 2 ||
        any(!is.finite(bounds)) ||
        bounds[1] >= bounds[2]) {
        stop(paste(
            "Invalid interval for", flag,
            "- expected two increasing numeric bounds, e.g. 0,100"
        ))
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

validate_config <- function(config) {
    n <- length(config$datasets)

    if (n == 0) stop("No datasets provided")
    if (n != length(config$dataset_names)) {
        stop("You need to set a name to your dataset\n",
             "format: graph.r {Dataset.csv} {Title} [flags]")
    }
    if (config$do_intersect && is.null(config$fit_models)) {
        stop("--intersect compares fitted curves, so it needs --fit")
    }
    if ((config$do_intersect || config$do_intersect_data) && n < 2) {
        stop("Intersections need at least two datasets")
    }

    if (!is.null(config$bars)) {
        if (!config$bars %in% BAR_MODES) {
            stop(paste("--bars expects one of:", paste(BAR_MODES, collapse = ", ")))
        }
        if (!is.null(config$fit_models) || config$do_intersect ||
            config$do_intersect_data) {
            stop("--bars cannot be combined with --fit, --intersect or --intersect-data")
        }
        if (config$bars == "normalize") {
            if (is.null(config$baseline)) config$baseline <- config$dataset_names[1]
            if (!config$baseline %in% config$dataset_names) {
                stop(paste("--baseline must be one of:",
                           paste(config$dataset_names, collapse = ", ")))
            }
        }
    } else if (!is.null(config$baseline)) {
        stop("--baseline only makes sense with --bars normalize")
    }

    config
}

parse_args <- function(args) {
    if (length(args) == 0) stop("No input file provided")

    config <- default_config()
    index  <- 1

    while (index <= length(args)) {
        arg  <- args[index]
        spec <- FLAG_SPECS[[arg]]

        if (!is.null(spec)) {
            if (length(config$datasets) > length(config$dataset_names)) {
                stop("You need to set a name to your dataset\n",
                     "format: graph.r {Dataset.csv} {Title} [flags]")
            }

            if (spec$type == "switch") {
                for (key in spec$keys) config[[key]] <- TRUE
            } else {
                if (index >= length(args)) stop(paste(arg, "requires a value"))
                index <- index + 1
                config[[spec$keys]] <- parse_flag_value(spec$type, args[index], arg)
            }
        } else if (length(config$datasets) == length(config$dataset_names)) {
            config$datasets <- c(config$datasets, arg)           # a path
        } else {
            config$dataset_names <- c(config$dataset_names, arg) # its name
        }

        index <- index + 1
    }

    validate_config(config)
}


# ------------------------------------------------------------------ data ----

load_data <- function(config) {
    frames <- lapply(seq_along(config$datasets), function(i) {
        d <- read.csv(config$datasets[i], sep = ";", header = TRUE)

        missing_cols <- setdiff(c(config$x_axis, config$y_axis), names(d))
        if (length(missing_cols) > 0) {
            stop(sprintf("%s: missing column(s) %s", config$datasets[i],
                         paste(missing_cols, collapse = ", ")))
        }

        d$Operation <- config$dataset_names[i]
        d
    })

    tibble(bind_rows(frames))
}

# Matrix of y values: one row per x shared by ALL datasets, one column per
# dataset. Repeated measurements at the same x are averaged.
aligned_matrix <- function(df, config) {
    per_dataset <- lapply(config$dataset_names, function(n) {
        d <- df[df$Operation == n, ]
        d <- d[is.finite(d[[config$x_axis]]) & is.finite(d[[config$y_axis]]), ]
        mean_y <- tapply(d[[config$y_axis]], d[[config$x_axis]], mean)
        list(x = as.numeric(names(mean_y)), y = as.vector(mean_y))
    })

    x <- sort(Reduce(intersect, lapply(per_dataset, function(p) p$x)))
    if (!is.null(config$x_interval)) {
        x <- x[x >= config$x_interval[1] & x <= config$x_interval[2]]
    }

    y <- sapply(per_dataset, function(p) p$y[match(x, p$x)])
    y <- matrix(y, nrow = length(x), dimnames = list(NULL, config$dataset_names))

    list(x = x, y = y)
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

# One fit per dataset, returned as a named list.
fit_datasets <- function(df, config) {
    n      <- length(config$dataset_names)
    models <- config$fit_models

    if (length(models) == 1) models <- rep(models, n)
    if (length(models) != n) {
        stop("--fit needs one model per dataset (or a single one for all)")
    }

    fits <- lapply(seq_len(n), function(i) {
        d <- df[df$Operation == config$dataset_names[i], ]
        fit_model(d[[config$x_axis]], d[[config$y_axis]], models[i],
                  config$x_interval)
    })
    names(fits) <- config$dataset_names

    for (i in seq_len(n)) {
        message(sprintf("%s: %s fit, R^2 = %.4f",
                        config$dataset_names[i], models[i], fits[[i]]$r2))
    }

    fits
}

# x values where the fitted curves are evaluated (and drawn).
fit_grid <- function(df, fits, config, n_points = 500) {
    x_rng <- if (!is.null(config$x_interval)) config$x_interval
             else range(df[[config$x_axis]])

    models <- vapply(fits, function(f) f$model, character(1))
    if (any(models %in% c("log", "nlogn", "power")) || config$log_x) {
        positive <- df[[config$x_axis]][df[[config$x_axis]] > 0]
        x_rng[1] <- max(x_rng[1], min(positive))
    }

    if (config$log_x) {
        exp(seq(log(x_rng[1]), log(x_rng[2]), length.out = n_points))
    } else {
        seq(x_rng[1], x_rng[2], length.out = n_points)
    }
}

fit_curves <- function(fits, xs) {
    bind_rows(lapply(names(fits), function(n) {
        tibble(x = xs, y = fits[[n]]$f(xs), Operation = n)
    }))
}


# ------------------------------------------------- "who is best" changes ----

# Given a matrix of y values (rows = increasing x, columns = candidates), finds
# the rows where the candidate with the LOWEST y changes.
#
# min_run > 1 ignores stretches where a candidate wins for fewer than min_run
# consecutive rows (they get absorbed by the previous stretch).
#
# Returns a tibble: index (row where the new winner starts), from, to (columns).
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
        j <- if (i == 1) 2 else i - 1
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

format_number <- function(x) {
    format(round(x, 2), scientific = FALSE, trim = TRUE)
}

build_marks <- function(x, y, from, to, names) {
    tibble(x     = x,
           y     = y,
           from  = names[from],
           to    = names[to],
           label = paste0("  x = ", format_number(x)))
}

# Winner changes measured on the data points themselves. The highlighted point
# is the first measured x where the new winner takes over.
data_marks <- function(df, config) {
    m  <- aligned_matrix(df, config)
    if (length(m$x) < 2) {
        message("Not enough x values shared by all datasets to compare.")
        return(NULL)
    }

    ch <- winner_changes(m$y, config$min_run)
    build_marks(m$x[ch$index], m$y[cbind(ch$index, ch$to)],
                ch$from, ch$to, config$dataset_names)
}

# Winner changes between the fitted curves. Each crossing is refined with
# uniroot, so it does not depend on the grid resolution.
fit_marks <- function(fits, xs) {
    y  <- sapply(fits, function(f) f$f(xs))
    ch <- winner_changes(y)

    roots <- vapply(seq_len(nrow(ch)), function(k) {
        before <- fits[[ch$from[k]]]$f
        after  <- fits[[ch$to[k]]]$f
        uniroot(function(x) before(x) - after(x),
                c(xs[ch$index[k] - 1], xs[ch$index[k]]))$root
    }, numeric(1))

    ys <- vapply(seq_along(roots), function(k) fits[[ch$to[k]]]$f(roots[k]),
                 numeric(1))

    build_marks(roots, ys, ch$from, ch$to, names(fits))
}

report_marks <- function(marks, what) {
    if (is.null(marks) || nrow(marks) == 0) {
        message(sprintf("%s: the best (lowest) one never changes.", what))
        return(invisible())
    }
    for (k in seq_len(nrow(marks))) {
        message(sprintf("%s: %s becomes best (over %s) at x = %.4f, y = %.4f",
                        what, marks$to[k], marks$from[k], marks$x[k], marks$y[k]))
    }
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

# Same rules for both axes.
axis_scale <- function(scale_fn, use_log, zero, interval, top = 0.05) {
    scale_fn(
        trans = if (use_log) "log10" else "identity",
        limits = if (!is.null(interval)) {
            interval
        } else if (zero) {
            c(if (use_log) 1 else 0, NA)
        } else {
            c(NA, NA)
        },
        expand = if (!is.null(interval)) {
            expansion(mult = 0)
        } else if (zero) {
            expansion(mult = c(0, top))
        } else {
            waiver()
        },
        breaks       = if (use_log) log_major_breaks else waiver(),
        minor_breaks = if (use_log) log_minor_breaks else waiver(),
        labels       = if (use_log) scales::label_log() else waiver()
    )
}

add_marks <- function(plt, marks) {
    if (is.null(marks) || nrow(marks) == 0) return(plt)

    plt +
        geom_vline(data = marks, aes(xintercept = x),
                   linetype = "dashed", color = "grey40") +
        geom_point(data = marks, aes(x = x, y = y), inherit.aes = FALSE,
                   size = 2.5, shape = 21, fill = "white",
                   color = "black", stroke = 0.9) +
        geom_text(data = marks, aes(x = x, y = y, label = label),
                  inherit.aes = FALSE, hjust = 0, vjust = -1)
}

build_plot <- function(df, config, curves = NULL, marks = NULL) {
    plt <- ggplot(df, aes(
        x     = .data[[config$x_axis]],
        y     = .data[[config$y_axis]],
        color = Operation,
        shape = Operation
    )) + geom_point()

    if (!is.null(curves)) {
        plt <- plt + geom_line(data = curves, aes(x = x, y = y, color = Operation),
                               inherit.aes = FALSE)
    }

    plt <- plt +
        axis_scale(scale_x_continuous, config$log_x, config$x_zero, config$x_interval) +
        axis_scale(scale_y_continuous, config$log_y, config$y_zero, config$y_interval) +
        labs(title = config$graph_title)

    add_marks(plt, marks)
}


# ------------------------------------------------------------ bar chart -----

# One row per (dataset, x). Repeated measurements are averaged. In normalize
# mode y becomes the ratio to the baseline dataset at the same x, and x values
# the baseline does not have are dropped.
bar_data <- function(df, config) {
    d <- df
    d$x <- d[[config$x_axis]]
    d$y <- d[[config$y_axis]]
    d <- d[is.finite(d$x) & is.finite(d$y), ]

    if (!is.null(config$x_interval)) {
        d <- d[d$x >= config$x_interval[1] & d$x <= config$x_interval[2], ]
    }

    d <- d %>%
        group_by(Operation, x) %>%
        summarise(y = mean(y), .groups = "drop")

    if (config$bars == "normalize") {
        base <- d[d$Operation == config$baseline, c("x", "y")]
        names(base)[2] <- "base"

        lost <- setdiff(unique(d$x), base$x)
        if (length(lost) > 0) {
            message("Baseline has no data for x = ", paste(lost, collapse = ", "),
                    "; those groups were dropped.")
        }

        d      <- inner_join(d, base, by = "x")
        d$y    <- d$y / d$base
        d$label <- paste0(round(d$y, 1), "x")
    }

    sizes       <- sort(unique(d$x))
    d$Size      <- factor(d$x, levels = sizes,
                          labels = format(sizes, scientific = FALSE, trim = TRUE))
    d$Operation <- factor(d$Operation, levels = config$dataset_names)
    d
}

build_bar_plot <- function(d, config) {
    normalize <- config$bars == "normalize"
    dodge     <- position_dodge(width = 0.9)

    plt <- ggplot(d, aes(x = Size, y = y, fill = Operation)) +
        geom_col(position = dodge, width = 0.9)

    if (normalize) {
        plt <- plt +
            geom_hline(yintercept = 1, linetype = "dotted", color = "grey40") +
            geom_text(aes(label = label), position = dodge, vjust = -0.4, size = 3.2)
    }

    plt +
        axis_scale(scale_y_continuous, config$bars == "log", TRUE,
                   config$y_interval, top = if (normalize) 0.1 else 0.05) +
        coord_cartesian(clip = "off") +
        labs(title = config$graph_title,
             x     = config$x_axis,
             y     = if (normalize) {
                 paste0(config$y_axis, " (relative to ", config$baseline, ")")
             } else {
                 config$y_axis
             })
}


# ------------------------------------------------------------------ main ----

# Points (and optionally fitted curves and "best changes" markers).
line_figure <- function(df, config) {
    curves <- NULL
    marks  <- NULL

    if (!is.null(config$fit_models)) {
        fits   <- fit_datasets(df, config)
        xs     <- fit_grid(df, fits, config)
        curves <- fit_curves(fits, xs)

        if (config$do_intersect) {
            found <- fit_marks(fits, xs)
            report_marks(found, "Fit")
            marks <- bind_rows(marks, found)
        }
    }

    if (config$do_intersect_data) {
        found <- data_marks(df, config)
        report_marks(found, "Data")
        marks <- bind_rows(marks, found)
    }

    build_plot(df, config, curves, marks)
}

main <- function(args) {
    config <- parse_args(args)

    suppressPackageStartupMessages({
        library(ggplot2)
        library(tibble)
        library(dplyr)
    })

    df  <- load_data(config)
    plt <- if (is.null(config$bars)) {
        line_figure(df, config)
    } else {
        build_bar_plot(bar_data(df, config), config)
    }

    ggsave(config$output_file, plt, device = "png")
}

if (!interactive()) main(commandArgs(trailingOnly = TRUE))
