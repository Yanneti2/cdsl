#!/usr/bin/env Rscript
#
# Smooth ("true") heatmap of break-even results: density x size.
# Self-contained: does not depend on any other script.
#
# Each input file holds one structure at one density, with the columns
#     "Size";"x"
# where x is the signed break-even already computed against the naive bit vector:
#     x <  0  -> naive is better (the structure never pays off)
#     x >= 0  -> the structure pays off after ~x operations (and above)
# Decimal comma or dot are both accepted.
#
# How the plane is painted:
#   1. each structure's x is interpolated (bilinear, signed-log space) on a fine grid;
#   2. each pixel gets the color of the structure that pays off with the FEWEST
#      operations there (smallest interpolated x >= 0);
#   3. green (naive) = data exists but no structure pays off; blue (none) = no data;
#   4. by default the color gets lighter as more operations are needed.
#
# Uso:
#   Rscript smooth_heat.r {Comparacao.csv} {Nome} [{Comparacao.csv} {Nome} ...] [flags]
#
# A densidade vem da coluna "Density" (se existir) ou do nome do arquivo
# ("-d30", "-d.1", "-d99.9").
#
# Flags:
#   --order a,b,c        estruturas, da mais "fria" à mais "quente" (default: bs,jacobson,rrr)
#   --max-size N         tamanho máximo (default: 10000)
#   --sizes a,b,c        tamanhos alvo (default: 64,128,...,8192)
#   --densities a,b,c    densidades (default: .1,1,5,10,30,50,70,90,95,99,99.9)
#   --x-axis COL         coluna do tamanho (default: Size)
#   --value-col COL      coluna do break-even (default: x, senão a 2ª coluna)
#   --density-col COL    coluna da densidade (default: Density)
#   --title/-t TXT       título
#   --output/-o ARQ      PNG (default: smooth_heatmap.png)
#   --res N              pixels por célula original (default: 30)
#   --no-shade           cor sólida por estrutura (sem clarear com o nº de operações)
#   --points             marca as medições reais

suppressPackageStartupMessages({
    library(ggplot2)
    library(dplyr)
})

# ---------------------------------------------------------------- args ----

parse_args <- function(args) {
    cfg <- list(
        datasets = character(), names = character(),
        order = c("bs", "jacobson", "rrr"),
        x_axis = "Size", value_col = "x", density_col = "Density",
        title = "Choosing Rank and Select Structures on Low Order Bit Vectors: Break-even Operation Count",
        output = "smooth_heatmap.png",
        max_size = 10000,
        sizes = c(64, 128, 256, 512, 1024, 2048, 4096, 8192),
        densities = c(.1, 1, 5, 10, 30, 50, 70, 90, 95, 99, 99.9),
        res = 30L, shade = TRUE, points = FALSE
    )
    flags <- list(
        "--order" = "order", "--max-size" = "max_size", "--sizes" = "sizes",
        "--densities" = "densities", "--x-axis" = "x_axis",
        "--value-col" = "value_col", "--density-col" = "density_col",
        "--title" = "title", "-t" = "title",
        "--output" = "output", "-o" = "output", "--res" = "res"
    )
    nums <- function(s) as.numeric(trimws(strsplit(s, ",")[[1]]))
    i <- 1
    while (i <= length(args)) {
        a <- args[i]
        if (a == "--no-shade") {
            cfg$shade <- FALSE
        } else if (a == "--points") {
            cfg$points <- TRUE
        } else if (!is.null(flags[[a]])) {
            if (i >= length(args)) stop(paste(a, "requer um valor"))
            i <- i + 1
            key <- flags[[a]]
            cfg[[key]] <- switch(key,
                order     = trimws(strsplit(args[i], ",")[[1]]),
                sizes     = nums(args[i]),
                densities = nums(args[i]),
                max_size  = as.numeric(args[i]),
                res       = as.integer(args[i]),
                args[i])
        } else if (length(cfg$datasets) == length(cfg$names)) {
            cfg$datasets <- c(cfg$datasets, a)
        } else {
            cfg$names <- c(cfg$names, a)
        }
        i <- i + 1
    }
    if (length(cfg$datasets) == 0) stop("Forneça os arquivos .csv")
    if (length(cfg$datasets) != length(cfg$names)) stop("Cada CSV precisa de um nome.")
    cfg
}

# --------------------------------------------------------------- loading ----

to_num <- function(v) as.numeric(gsub(",", ".", trimws(v), fixed = TRUE))

load_one <- function(path, name, cfg) {
    raw <- read.table(path, sep = ";", header = TRUE, quote = "\"",
                      colClasses = "character", check.names = FALSE,
                      stringsAsFactors = FALSE)
    if (ncol(raw) < 2) stop(sprintf("%s: esperado ao menos 2 colunas (Size;x).", path))

    size_col <- if (cfg$x_axis %in% names(raw)) cfg$x_axis else names(raw)[1]
    val_col  <- if (cfg$value_col %in% names(raw)) cfg$value_col else names(raw)[2]

    dens <- if (cfg$density_col %in% names(raw)) {
        to_num(raw[[cfg$density_col]])
    } else {
        m <- regmatches(basename(path), regexpr("[-_]d([0-9]+(\\.[0-9]+)?|\\.[0-9]+)", basename(path)))
        if (length(m) == 0) stop(sprintf("%s: densidade não encontrada (coluna ou '-dNN' / '-d.1' no nome).", path))
        rep(as.numeric(sub("[-_]d", "", m)), nrow(raw))   # "d.1" -> 0.1
    }

    data.frame(
        Structure = name,
        Size      = to_num(raw[[size_col]]),
        Density   = round(dens, 3),
        x         = to_num(raw[[val_col]]),
        stringsAsFactors = FALSE
    )
}

# Tamanhos-alvo: o existente mais próximo (escala log) de cada tamanho pedido.
select_sizes <- function(sizes, cfg) {
    u <- sort(unique(sizes[is.finite(sizes) & sizes > 0 & sizes <= cfg$max_size]))
    if (length(u) == 0) stop("Nenhum tamanho <= --max-size nos datasets.")
    tg <- cfg$sizes[cfg$sizes <= cfg$max_size]
    unique(sapply(tg, function(s) u[which.min(abs(log(u) - log(s)))]))
}

load_all <- function(cfg) {
    all  <- bind_rows(lapply(seq_along(cfg$datasets), function(i) load_one(cfg$datasets[i], cfg$names[i], cfg)))
    keep <- select_sizes(all$Size, cfg)
    df <- all %>%
        filter(Size %in% keep) %>%
        group_by(Structure, Size, Density) %>%
        summarise(x = mean(x, na.rm = TRUE), .groups = "drop") %>%
        mutate(x = ifelse(is.nan(x), NA_real_, x))
    attr(df, "sizes") <- keep
    df
}

# Grade completa (estrutura x tamanho x densidade); o que não tem dado vira no_data.
build_results <- function(df, cfg) {
    grid <- expand.grid(
        Structure = unique(df$Structure), Size = attr(df, "sizes"),
        Density = round(cfg$densities, 3), stringsAsFactors = FALSE
    )
    grid %>%
        left_join(df, by = c("Structure", "Size", "Density")) %>%
        mutate(status = case_when(is.na(x) ~ "no_data", x >= 0 ~ "pays_off", TRUE ~ "never"))
}

# ----------------------------------------------------------- interpolation ----

sgnlog     <- function(x) sign(x) * log1p(abs(x))
inv_sgnlog <- function(t) sign(t) * expm1(abs(t))

# Matriz [tamanho x densidade] por estrutura, no espaço signed-log.
struct_matrices <- function(res, sizes, dens, structs) {
    lapply(setNames(structs, structs), function(s) {
        M <- matrix(NA_real_, length(sizes), length(dens))
        d <- res[res$Structure == s & !is.na(res$x), ]
        if (nrow(d) > 0) M[cbind(match(d$Size, sizes), match(d$Density, dens))] <- sgnlog(d$x)
        M
    })
}

# Interpolação bilinear (NA se qualquer um dos 4 cantos for NA).
bilinear <- function(M, xpos, ypos, xs, ys) {
    j  <- findInterval(xs, xpos, all.inside = TRUE)
    i  <- findInterval(ys, ypos, all.inside = TRUE)
    tx <- (xs - xpos[j]) / (xpos[j + 1] - xpos[j])
    ty <- (ys - ypos[i]) / (ypos[i + 1] - ypos[i])
    out <- matrix(NA_real_, length(ys), length(xs))
    for (a in seq_along(ys)) {
        r0 <- (1 - tx) * M[i[a], j]     + tx * M[i[a], j + 1]
        r1 <- (1 - tx) * M[i[a] + 1, j] + tx * M[i[a] + 1, j + 1]
        out[a, ] <- (1 - ty[a]) * r0 + ty[a] * r1
    }
    out
}

# ------------------------------------------------------------------- plot ----

format_n <- function(n) {
    ifelse(n >= 1e6, sprintf("%.1fM", n / 1e6),
    ifelse(n >= 1e4, sprintf("%.0fk", n / 1e3),
    ifelse(n >= 1e3, sprintf("%.1fk", n / 1e3), sprintf("%.0f", n))))
}

STRUCT_COLORS <- c(bs = "#f1c40f", jacobson = "#e67e22", rrr = "#d32f2f")

make_palette <- function(present, cfg) {
    lv  <- c(cfg$order[cfg$order %in% present], setdiff(present, cfg$order))
    pal <- setNames(rep(NA_character_, length(lv)), lv)
    known <- intersect(lv, names(STRUCT_COLORS))
    pal[known] <- STRUCT_COLORS[known]
    other <- names(pal)[is.na(pal)]
    if (length(other) > 0) pal[other] <- colorRampPalette(c("#f1c40f", "#d32f2f"))(length(other))
    list(lv = lv, pal = c(naive = "#2ecc71", pal, none = "#3498db"))
}

build_smooth_plot <- function(res, cfg) {
    sizes <- sort(unique(res$Size))
    dens  <- sort(unique(res$Density))
    if (length(sizes) < 2 || length(dens) < 2) stop("São necessários ao menos 2 tamanhos e 2 densidades.")

    pp      <- make_palette(unique(res$Structure), cfg)
    structs <- pp$lv
    pal     <- pp$pal

    xpos <- seq_along(dens)           # densidades igualmente espaçadas (como colunas)
    ypos <- log2(sizes)               # tamanhos em escala log2
    xs <- seq(1, length(dens), length.out = (length(dens) - 1) * cfg$res + 1)
    ys <- seq(min(ypos), max(ypos), length.out = (length(sizes) - 1) * cfg$res + 1)

    mats <- struct_matrices(res, sizes, dens, structs)

    best <- matrix(Inf, length(ys), length(xs))
    win  <- matrix(0L,  length(ys), length(xs))
    any_data <- matrix(FALSE, length(ys), length(xs))
    for (k in seq_along(structs)) {
        Fk <- inv_sgnlog(bilinear(mats[[structs[k]]], xpos, ypos, xs, ys))
        any_data <- any_data | !is.na(Fk)
        cand <- ifelse(!is.na(Fk) & Fk >= 0, Fk, Inf)
        better <- cand < best
        best[better] <- cand[better]
        win[better]  <- k
    }

    category <- ifelse(win > 0, structs[pmax(win, 1L)], ifelse(any_data, "naive", "none"))
    nops <- pmax(ceiling(best), 1)

    lighten <- matrix(0, length(ys), length(xs))
    if (cfg$shade && any(win > 0)) {
        nmax <- max(10, max(nops[win > 0]))
        lighten <- ifelse(win > 0, 0.55 * pmin(1, log10(nops) / log10(nmax)), 0)
    }

    px <- expand.grid(y = ys, x = xs)              # y varia mais rápido = ordem de as.vector()
    rgbm <- col2rgb(pal[as.vector(category)])
    L <- as.vector(lighten)
    px$col <- rgb(rgbm[1, ] * (1 - L) + 255 * L,
                  rgbm[2, ] * (1 - L) + 255 * L,
                  rgbm[3, ] * (1 - L) + 255 * L, maxColorValue = 255)

    leg <- data.frame(Structure = factor(c("naive", structs, "none"), levels = c("naive", structs, "none")),
                      x = 1, y = min(ypos))

    p <- ggplot() +
        geom_raster(data = px, aes(x = x, y = y, fill = col)) +
        scale_fill_identity() +
        geom_point(data = leg, aes(x = x, y = y, color = Structure), alpha = 0) +
        scale_color_manual(values = pal, drop = FALSE,
                           labels = function(v) ifelse(v == "none", "none (no data)", v)) +
        guides(color = guide_legend(override.aes = list(alpha = 1, shape = 15, size = 6))) +
        scale_x_continuous(breaks = xpos, expand = c(0, 0),
                           labels = format(dens, trim = TRUE, drop0trailing = TRUE)) +
        scale_y_continuous(breaks = ypos, expand = c(0, 0), labels = format_n(sizes)) +
        labs(
            title    = cfg$title,
            subtitle = paste0("color: structure that pays off with the fewest operations (naive if none does)",
                              if (cfg$shade) " | lighter = more operations needed" else ""),
            x = "Density (%)", y = "Bit vector size (bits)", color = "Best choice"
        ) +
        theme_minimal(base_size = 12) +
        theme(panel.grid = element_blank(),
              panel.border = element_rect(fill = NA, colour = "black"),
              plot.title = element_text(face = "bold", size = 12),
              plot.subtitle = element_text(size = 8))

    if (cfg$points) {
        pts <- res %>% filter(status != "no_data") %>% distinct(Size, Density)
        pts <- data.frame(x = match(pts$Density, dens), y = log2(pts$Size))
        p <- p + geom_point(data = pts, aes(x = x, y = y), shape = 21, size = 1.3,
                            fill = "white", color = "black", stroke = 0.3)
    }
    p
}

# ------------------------------------------------------------------- main ----

main <- function(args) {
    cfg <- parse_args(args)
    df  <- load_all(cfg)

    expected <- round(cfg$densities, 3)
    found    <- sort(unique(df$Density))
    if (length(setdiff(found, expected)) > 0)
        cat("Aviso: densidades nos arquivos fora de --densities (ignoradas):",
            paste(setdiff(found, expected), collapse = ", "), "\n")
    if (length(setdiff(expected, found)) > 0)
        cat("Aviso: densidades SEM nenhum arquivo (regiões n/a):",
            paste(setdiff(expected, found), collapse = ", "), "\n")

    res <- build_results(df, cfg)
    ggsave(cfg$output, build_smooth_plot(res, cfg), width = 10, height = 5.5, dpi = 300)
    cat(sprintf("Gráfico salvo com sucesso em: %s\n", cfg$output))
}

if (sys.nframe() == 0L) main(commandArgs(trailingOnly = TRUE))