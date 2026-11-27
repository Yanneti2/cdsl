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
#   1. each structure's 1/x (payoff rate) is interpolated (bilinear) on a fine grid;
#   2. each pixel gets the color of the structure that pays off with the FEWEST
#      operations there (smallest interpolated x >= 0);
#   3. light grey (naive) = data exists but no structure pays off; medium grey (none) = no data;
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
#   --memory ARQ         CSV com overhead de memória (Structure;Size;Density;Overhead)
#   --memory-contours ARQ PNG do heatmap com curvas de nível de overhead
#   --memory-output ARQ  PNG do heatmap contínuo de overhead de memória
#   --memory-3d ARQ      PNG da superfície 3D (Z = overhead, cor = estrutura + ops)
#   --theta DEG          ângulo horizontal da câmera 3D (default: -35)
#   --phi DEG            ângulo vertical da câmera 3D (default: 28)

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
        res = 30L, shade = TRUE, points = FALSE,
        memory = "", memory_contours = "", memory_output = "", memory_3d = "",
        theta = -35, phi = 28
    )
    flags <- list(
        "--order" = "order", "--max-size" = "max_size", "--sizes" = "sizes",
        "--densities" = "densities", "--x-axis" = "x_axis",
        "--value-col" = "value_col", "--density-col" = "density_col",
        "--title" = "title", "-t" = "title",
        "--output" = "output", "-o" = "output", "--res" = "res",
        "--memory" = "memory", "--memory-contours" = "memory_contours",
        "--memory-output" = "memory_output", "--memory-3d" = "memory_3d",
        "--theta" = "theta", "--phi" = "phi"
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
                theta     = as.numeric(args[i]),
                phi       = as.numeric(args[i]),
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

# Taxa de retorno (1/x = (Time_naive - Time_struct) / Build_time):
# continua no cruzamento de "nunca compensa" (x < 0) para "compensa" (x > 0),
# passando por 1/x = 0 (x = Inf) em vez de criar um falso x = 0.
to_rate <- function(x) ifelse(x == 0, 1e9, 1 / x)

# Matriz [tamanho x densidade] por estrutura, no espaço 1/x.
struct_matrices <- function(res, sizes, dens, structs) {
    lapply(setNames(structs, structs), function(s) {
        M <- matrix(NA_real_, length(sizes), length(dens))
        d <- res[res$Structure == s & !is.na(res$x), ]
        if (nrow(d) > 0) M[cbind(match(d$Size, sizes), match(d$Density, dens))] <- to_rate(d$x)
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

STRUCT_COLORS <- c(bitvectorBS = "#F6921E", bs = "#F6921E",
                   jacobson = "#112A52", rrr = "#8c4a12")

EXTRA_COLORS  <- c("#5B7DB1", "#8C4A12", "#FBC47F")
NAVY <- "#112A52"
INK  <- "#29323D"

make_palette <- function(present, cfg) {
    lv  <- c(cfg$order[cfg$order %in% present], setdiff(present, cfg$order))
    pal <- setNames(rep(NA_character_, length(lv)), lv)
    known <- intersect(lv, names(STRUCT_COLORS))
    pal[known] <- STRUCT_COLORS[known]
    other <- names(pal)[is.na(pal)]
    if (length(other) > 0) pal[other] <- rep(EXTRA_COLORS, length.out = length(other))
    list(lv = lv, pal = c(naive = "#38646b", pal, none = "#BDBDBD"))
}

format_overhead <- function(v) {
    ifelse(is.na(v), "n/a",
    ifelse(round(v) == 0, "0%",
           sprintf("%+.0f%%", v)))
}

load_memory <- function(path) {
    raw <- read.table(path, sep = ";", header = TRUE,
                      colClasses = "character", check.names = FALSE,
                      stringsAsFactors = FALSE)
    data.frame(Structure = raw$Structure,
               Size      = to_num(raw$Size),
               Density   = round(to_num(raw$Density), 3),
               Overhead  = to_num(raw$Overhead),
               stringsAsFactors = FALSE) %>%
        group_by(Structure, Size, Density) %>%
        summarise(Overhead = mean(Overhead, na.rm = TRUE), .groups = "drop")
}

compute_smooth_grid <- function(res, cfg, res_override = NULL) {
    sizes <- sort(unique(res$Size))
    dens  <- sort(unique(res$Density))
    if (length(sizes) < 2 || length(dens) < 2) stop("São necessários ao menos 2 tamanhos e 2 densidades.")

    pp      <- make_palette(unique(res$Structure), cfg)
    structs <- pp$lv
    pal     <- pp$pal

    step_res <- if (is.null(res_override)) cfg$res else res_override
    xpos <- seq_along(dens)           # densidades igualmente espaçadas (como colunas)
    ypos <- log2(sizes)               # tamanhos em escala log2
    xs <- seq(1, length(dens), length.out = (length(dens) - 1) * step_res + 1)
    ys <- seq(min(ypos), max(ypos), length.out = (length(sizes) - 1) * step_res + 1)

    mats <- struct_matrices(res, sizes, dens, structs)

    best <- matrix(Inf, length(ys), length(xs))
    win  <- matrix(0L,  length(ys), length(xs))
    any_data <- matrix(FALSE, length(ys), length(xs))
    for (k in seq_along(structs)) {
        Rk <- bilinear(mats[[structs[k]]], xpos, ypos, xs, ys)
        any_data <- any_data | !is.na(Rk)
        cand <- ifelse(!is.na(Rk) & Rk > 0, 1 / Rk, Inf)
        better <- cand < best
        best[better] <- cand[better]
        win[better]  <- k
    }

    category <- ifelse(win > 0, structs[pmax(win, 1L)], ifelse(any_data, "naive", "none"))
    nops <- pmax(ceiling(best), 1)

    lighten <- matrix(0, length(ys), length(xs))
    if (cfg$shade && any(win > 0)) {
        obs_pos <- res$x[!is.na(res$x) & res$x > 0]
        nmax <- max(10, if (length(obs_pos) > 0) max(ceiling(obs_pos)) else max(nops[win > 0 & is.finite(nops)]))
        lighten <- ifelse(win > 0, 0.55 * pmin(1, log10(nops) / log10(nmax)), 0)
    }

    px <- expand.grid(y = ys, x = xs)              # y varia mais rápido = ordem de as.vector()
    rgbm <- col2rgb(pal[as.vector(category)])
    L <- as.vector(lighten)
    px$col <- rgb(rgbm[1, ] * (1 - L) + 255 * L,
                  rgbm[2, ] * (1 - L) + 255 * L,
                  rgbm[3, ] * (1 - L) + 255 * L, maxColorValue = 255)

    list(
        sizes = sizes, dens = dens, xpos = xpos, ypos = ypos,
        xs = xs, ys = ys, structs = structs, pal = pal,
        best = best, win = win, any_data = any_data,
        category = category, nops = nops, lighten = lighten, px = px
    )
}

compute_memory_matrix <- function(g, mem) {
    mem_mats <- lapply(setNames(g$structs, g$structs), function(s) {
        M <- matrix(NA_real_, length(g$sizes), length(g$dens))
        d <- mem[mem$Structure == s & !is.na(mem$Overhead), ]
        if (nrow(d) > 0) {
            r <- match(d$Size, g$sizes)
            c <- match(d$Density, g$dens)
            ok <- !is.na(r) & !is.na(c)
            if (any(ok)) M[cbind(r[ok], c[ok])] <- d$Overhead[ok]
        }
        M
    })

    interp <- lapply(mem_mats, function(M) bilinear(M, g$xpos, g$ypos, g$xs, g$ys))

    Z <- matrix(NA_real_, length(g$ys), length(g$xs))
    Z[g$any_data & g$win == 0L] <- 0
    for (k in seq_along(g$structs)) {
        mask <- g$win == k
        if (any(mask)) Z[mask] <- interp[[g$structs[k]]][mask]
    }
    Z
}

build_smooth_plot <- function(res, cfg, g = NULL) {
    if (is.null(g)) g <- compute_smooth_grid(res, cfg)

    leg <- data.frame(Structure = factor(c("naive", g$structs, "none"), levels = c("naive", g$structs, "none")),
                      x = 1, y = min(g$ypos))

    p <- ggplot() +
        geom_raster(data = g$px, aes(x = x, y = y, fill = col)) +
        scale_fill_identity() +
        geom_point(data = leg, aes(x = x, y = y, color = Structure), alpha = 0) +
        scale_color_manual(values = g$pal, drop = FALSE,
                           labels = function(v) ifelse(v == "none", "none (no data)", v)) +
        guides(color = guide_legend(override.aes = list(alpha = 1, shape = 15, size = 6))) +
        scale_x_continuous(breaks = g$xpos, expand = c(0, 0),
                           labels = format(g$dens, trim = TRUE, drop0trailing = TRUE)) +
        scale_y_continuous(breaks = g$ypos, expand = c(0, 0), labels = format_n(g$sizes)) +
        labs(
            title    = cfg$title,
            subtitle = paste0("color: structure that pays off with the fewest operations (naive if none does)",
                              if (cfg$shade) " | lighter = more operations needed" else ""),
            x = "Density (%)", y = "Bit vector size (bits)", color = "Best choice"
        ) +
        theme_minimal(base_size = 12) +
        theme(text = element_text(color = INK),
              axis.text = element_text(color = INK),
              panel.grid = element_blank(),
              panel.border = element_rect(fill = NA, colour = INK),
              plot.title = element_text(color = NAVY, face = "bold", size = 12),
              plot.subtitle = element_text(size = 8))

    if (cfg$points) {
        pts <- res %>% filter(status != "no_data") %>% distinct(Size, Density)
        pts <- data.frame(x = match(pts$Density, g$dens), y = log2(pts$Size))
        p <- p + geom_point(data = pts, aes(x = x, y = y), shape = 21, size = 1.3,
                            fill = "white", color = "black", stroke = 0.3)
    }
    p
}

# Smooth heatmap + labeled iso-overhead contour lines
build_contour_plot <- function(res, cfg, g, mem) {
    Z <- compute_memory_matrix(g, mem)
    p <- build_smooth_plot(res, cfg, g)

    z_vals <- Z[is.finite(Z)]
    if (length(z_vals) < 2 || diff(range(z_vals)) == 0) return(p)

    cand_levels <- c(-75, -50, -25, 0, 100, 300, 600, 1000)
    zr <- range(z_vals)
    levels <- cand_levels[cand_levels > zr[1] + 1 & cand_levels < zr[2] - 1]
    if (length(levels) == 0) levels <- pretty(zr, n = 5)

    # contourLines expects z[i, j] matching x[i], y[j], so pass t(Z)
    cl <- grDevices::contourLines(x = g$xs, y = g$ys, z = t(Z), levels = levels)
    if (length(cl) == 0) return(p)

    line_df <- bind_rows(lapply(seq_along(cl), function(id) {
        data.frame(
            x = cl[[id]]$x,
            y = cl[[id]]$y,
            level = cl[[id]]$level,
            group = id,
            is_zero = cl[[id]]$level == 0
        )
    }))

    # Pick one clean label position per level (avoiding overlap on steep jumps)
    xr <- range(g$xs); yr <- range(g$ys)
    placed <- data.frame(nx = numeric(), ny = numeric())
    label_rows <- list()

    # Order levels so 0% and extremes get placed first
    by_lvl <- split(cl, sapply(cl, `[[`, "level"))
    lvl_keys <- names(by_lvl)
    ord <- order(abs(as.numeric(lvl_keys)))
    for (k in lvl_keys[ord]) {
        segs <- by_lvl[[k]]
        # Prefer longer segments away from boundaries
        seg_lens <- sapply(segs, function(s) length(s$x))
        segs <- segs[order(seg_lens, decreasing = TRUE)]
        best_pt <- NULL
        best_score <- -Inf
        for (s in segs) {
            if (length(s$x) < 12) next
            idx <- seq(max(3, floor(length(s$x) * 0.15)),
                       min(length(s$x) - 2, ceiling(length(s$x) * 0.85)),
                       length.out = min(15, length(s$x)))
            idx <- unique(round(idx))
            for (j in idx) {
                nx <- (s$x[j] - xr[1]) / diff(xr)
                ny <- (s$y[j] - yr[1]) / diff(yr)
                if (nx < 0.06 || nx > 0.94 || ny < 0.06 || ny > 0.94) next
                dmin <- if (nrow(placed) == 0) 1.0 else min(sqrt((placed$nx - nx)^2 + (placed$ny - ny)^2))
                if (dmin > best_score) {
                    best_score <- dmin
                    best_pt <- list(x = s$x[j], y = s$y[j], nx = nx, ny = ny, level = s$level)
                }
            }
        }
        if (!is.null(best_pt) && best_score >= 0.085) {
            placed <- rbind(placed, data.frame(nx = best_pt$nx, ny = best_pt$ny))
            label_rows[[length(label_rows) + 1L]] <- data.frame(
                x = best_pt$x, y = best_pt$y,
                label = format_overhead(best_pt$level),
                stringsAsFactors = FALSE
            )
        }
    }

    p <- p +
        geom_path(data = line_df, aes(x = x, y = y, group = group),
                  color = "white", linewidth = 0.85, alpha = 0.7) +
        geom_path(data = line_df %>% filter(!is_zero), aes(x = x, y = y, group = group),
                  color = INK, linewidth = 0.38, linetype = "dashed", alpha = 0.9) +
        geom_path(data = line_df %>% filter(is_zero), aes(x = x, y = y, group = group),
                  color = INK, linewidth = 0.55, linetype = "solid", alpha = 0.95) +
        labs(
            subtitle = paste0("color: best structure (naive if none)",
                              if (cfg$shade) " | lighter = more ops" else "",
                              " | contours: memory overhead vs naive (solid = 0%)")
        )

    if (length(label_rows) > 0) {
        lab_df <- bind_rows(label_rows)
        p <- p + geom_label(
            data = lab_df, aes(x = x, y = y, label = label),
            size = 2.5, fontface = "bold", color = INK,
            fill = grDevices::adjustcolor("white", alpha.f = 0.88),
            label.padding = unit(0.14, "lines"),
            label.r = unit(0.15, "lines"),
            linewidth = 0.25
        )
    }
    p
}

# Companion smooth memory-overhead heatmap with winner boundaries & region labels
build_memory_smooth_plot <- function(res, cfg, g, mem) {
    Z <- compute_memory_matrix(g, mem)
    px <- expand.grid(y = g$ys, x = g$xs)
    px$Overhead <- as.vector(Z)
    px$Category <- as.vector(g$category)

    z_vals <- Z[is.finite(Z)]
    zmin <- min(0, min(z_vals))
    zmax <- max(0, max(z_vals))

    # Diverging scale centered at 0%: teal/green (<0% saves memory), white (0%), navy (>0% overhead)
    if (zmin < 0 && zmax > 0) {
        stops <- c(zmin, zmin * 0.35, 0, zmax * 0.25, zmax * 0.6, zmax)
        cols  <- c("#1B7837", "#74C476", "#F4F4F4", "#9ECAE1", "#31688E", "#112A52")
        fill_scale <- scale_fill_gradientn(
            colours = cols,
            values  = scales::rescale(stops, from = c(zmin, zmax)),
            limits  = c(zmin, zmax),
            na.value = "#BDBDBD"
        )
    } else if (zmax <= 0) {
        fill_scale <- scale_fill_gradient(low = "#1B7837", high = "#F4F4F4", na.value = "#BDBDBD")
    } else {
        fill_scale <- scale_fill_gradient(low = "#F4F4F4", high = "#112A52", na.value = "#BDBDBD")
    }

    # Winner boundary segments: where category changes between adjacent pixels
    ny <- length(g$ys); nx <- length(g$xs)
    dx <- diff(g$xs)[1] / 2; dy <- diff(g$ys)[1] / 2
    bnd_list <- list()
    # Vertical edges between col j and j+1
    diff_h <- g$category[, -nx, drop = FALSE] != g$category[, -1, drop = FALSE]
    if (any(diff_h)) {
        wh <- which(diff_h, arr.ind = TRUE)
        bnd_list[[1]] <- data.frame(
            x    = g$xs[wh[, 2]] + dx,
            xend = g$xs[wh[, 2]] + dx,
            y    = g$ys[wh[, 1]] - dy,
            yend = g$ys[wh[, 1]] + dy
        )
    }
    # Horizontal edges between row i and i+1
    diff_v <- g$category[-ny, , drop = FALSE] != g$category[-1, , drop = FALSE]
    if (any(diff_v)) {
        wv <- which(diff_v, arr.ind = TRUE)
        bnd_list[[length(bnd_list) + 1L]] <- data.frame(
            x    = g$xs[wv[, 2]] - dx,
            xend = g$xs[wv[, 2]] + dx,
            y    = g$ys[wv[, 1]] + dy,
            yend = g$ys[wv[, 1]] + dy
        )
    }
    bnd_df <- if (length(bnd_list) > 0) bind_rows(bnd_list) else NULL

    # Region labels at median point of each present winner category
    reg_df <- px %>%
        filter(Category != "none") %>%
        group_by(Category) %>%
        summarise(
            n = n(),
            x = median(x),
            y = median(y),
            .groups = "drop"
        ) %>%
        filter(n >= 40)

    p <- ggplot() +
        geom_raster(data = px, aes(x = x, y = y, fill = Overhead)) +
        fill_scale +
        scale_x_continuous(breaks = g$xpos, expand = c(0, 0),
                           labels = format(g$dens, trim = TRUE, drop0trailing = TRUE)) +
        scale_y_continuous(breaks = g$ypos, expand = c(0, 0), labels = format_n(g$sizes)) +
        labs(
            title    = paste0(cfg$title, " — Memory Overhead"),
            subtitle = "color: memory overhead (%) of the winning structure (<0% saves memory, 0% = naive) | lines: winner boundaries",
            x = "Density (%)", y = "Bit vector size (bits)", fill = "Overhead (%)"
        ) +
        theme_minimal(base_size = 12) +
        theme(text = element_text(color = INK),
              axis.text = element_text(color = INK),
              panel.grid = element_blank(),
              panel.border = element_rect(fill = NA, colour = INK),
              plot.title = element_text(color = NAVY, face = "bold", size = 12),
              plot.subtitle = element_text(size = 8))

    if (!is.null(bnd_df) && nrow(bnd_df) > 0) {
        p <- p +
            geom_segment(data = bnd_df, aes(x = x, y = y, xend = xend, yend = yend),
                         color = "white", linewidth = 0.9, alpha = 0.7) +
            geom_segment(data = bnd_df, aes(x = x, y = y, xend = xend, yend = yend),
                         color = INK, linewidth = 0.45)
    }

    if (nrow(reg_df) > 0) {
        p <- p + geom_label(
            data = reg_df, aes(x = x, y = y, label = Category),
            size = 3.0, fontface = "bold", color = INK,
            fill = grDevices::adjustcolor("white", alpha.f = 0.85),
            label.padding = unit(0.2, "lines"),
            linewidth = 0.3
        )
    }
    p
}

# 4D surface plot: X = Density, Y = Size (log2), Z = Memory Overhead (%), Color = winner + ops shading
save_3d_plot <- function(res, cfg, mem, out_path) {
    g3 <- compute_smooth_grid(res, cfg, res_override = 6L)
    Z_yx <- compute_memory_matrix(g3, mem)       # [length(ys) x length(xs)]
    Z_xy <- t(Z_yx)                              # persp expects [length(xs) x length(ys)]

    nx <- length(g3$xs); ny <- length(g3$ys)
    col_yx <- matrix(g3$px$col, nrow = ny, ncol = nx)
    col_xy <- t(col_yx)
    # Facet color = color at facet center (average of the 4 corners in winner space via lower-left or nearest)
    # Use the 2D color at the upper-left corner of each quad (visually identical at 6x subdivision)
    fcol <- col_xy[-nx, -ny, drop = FALSE]

    z_vals <- Z_xy[is.finite(Z_xy)]
    zmin <- min(0, floor(min(z_vals) / 100) * 100)
    zmax <- max(100, ceiling(max(z_vals) / 100) * 100)
    zlim <- c(zmin, zmax)

    grDevices::png(out_path, width = 12.0, height = 7.4, units = "in", res = 300)
    on.exit(grDevices::dev.off(), add = TRUE)

    old_par <- par(no.readonly = TRUE)
    on.exit(par(old_par), add = TRUE)
    par(mar = c(2.2, 3.0, 4.2, 8.5), bg = "white", col.main = NAVY, fg = INK)

    pmat <- graphics::persp(
        x = g3$xs, y = g3$ys, z = Z_xy,
        zlim = zlim,
        col = fcol,
        border = grDevices::adjustcolor(INK, alpha.f = 0.08),
        lwd = 0.3,
        theta = cfg$theta, phi = cfg$phi,
        expand = 0.55, scale = TRUE,
        box = TRUE, axes = FALSE,
        shade = 0.15, ltheta = -120, lphi = 45
    )

    # Subtle 0% reference perimeter box
    if (zlim[1] < 0 && zlim[2] > 0) {
        bx <- c(min(g3$xs), max(g3$xs), max(g3$xs), min(g3$xs), min(g3$xs))
        by <- c(min(g3$ys), min(g3$ys), max(g3$ys), max(g3$ys), min(g3$ys))
        p0 <- grDevices::trans3d(bx, by, rep(0, 5), pmat)
        lines(p0, col = grDevices::adjustcolor(INK, alpha.f = 0.45), lty = 2, lwd = 1.0)
    }

    # Draw subtle grid lines along actual Density & Size tick coordinates on the surface
    for (xi in g3$xpos) {
        j <- which.min(abs(g3$xs - xi))
        pts <- grDevices::trans3d(rep(g3$xs[j], ny), g3$ys, Z_xy[j, ], pmat)
        lines(pts, col = grDevices::adjustcolor("white", alpha.f = 0.35), lwd = 0.6)
    }
    for (yi in g3$ypos) {
        i <- which.min(abs(g3$ys - yi))
        pts <- grDevices::trans3d(g3$xs, rep(g3$ys[i], nx), Z_xy[, i], pmat)
        lines(pts, col = grDevices::adjustcolor("white", alpha.f = 0.35), lwd = 0.6)
    }

    # Custom axis ticks & labels via trans3d
    par(xpd = NA)
    x0 <- min(g3$xs); x1 <- max(g3$xs)
    y0 <- min(g3$ys); y1 <- max(g3$ys)
    z0 <- zlim[1];    z1 <- zlim[2]
    dx_span <- x1 - x0; dy_span <- y1 - y0

    # X axis (Density %) along front edge y = y0, z = z0
    x_ticks_start <- grDevices::trans3d(g3$xpos, rep(y0, length(g3$xpos)), rep(z0, length(g3$xpos)), pmat)
    x_ticks_end   <- grDevices::trans3d(g3$xpos, rep(y0 - 0.035 * dy_span, length(g3$xpos)), rep(z0, length(g3$xpos)), pmat)
    segments(x_ticks_start$x, x_ticks_start$y, x_ticks_end$x, x_ticks_end$y, col = INK, lwd = 0.8)
    x_lbl_pos <- grDevices::trans3d(g3$xpos, rep(y0 - 0.095 * dy_span, length(g3$xpos)), rep(z0, length(g3$xpos)), pmat)
    text(x_lbl_pos$x, x_lbl_pos$y, labels = format(g3$dens, trim = TRUE, drop0trailing = TRUE),
         cex = 0.72, col = INK)
    x_title_pos <- grDevices::trans3d(mean(c(x0, x1)), y0 - 0.21 * dy_span, z0, pmat)
    text(x_title_pos$x, x_title_pos$y, labels = "Density (%)", cex = 0.95, font = 2, col = INK)

    # Y axis (Size bits) along right-front edge x = x1, z = z0
    y_ticks_start <- grDevices::trans3d(rep(x1, length(g3$ypos)), g3$ypos, rep(z0, length(g3$ypos)), pmat)
    y_ticks_end   <- grDevices::trans3d(rep(x1 + 0.03 * dx_span, length(g3$ypos)), g3$ypos, rep(z0, length(g3$ypos)), pmat)
    segments(y_ticks_start$x, y_ticks_start$y, y_ticks_end$x, y_ticks_end$y, col = INK, lwd = 0.8)
    y_lbl_pos <- grDevices::trans3d(rep(x1 + 0.055 * dx_span, length(g3$ypos)), g3$ypos, rep(z0, length(g3$ypos)), pmat)
    text(y_lbl_pos$x, y_lbl_pos$y, labels = format_n(g3$sizes), cex = 0.72, col = INK, adj = 0)
    # Angle Y axis title parallel to the projected Y edge so it never overlaps tick labels
    p_y0 <- grDevices::trans3d(x1, y0, z0, pmat)
    p_y1 <- grDevices::trans3d(x1, y1, z0, pmat)
    y_ang <- atan2(p_y1$y - p_y0$y, p_y1$x - p_y0$x) * 180 / pi
    if (y_ang > 90) y_ang <- y_ang - 180
    if (y_ang < -90) y_ang <- y_ang + 180
    y_title_pos <- grDevices::trans3d(x1 + 0.22 * dx_span, mean(c(y0, y1)), z0, pmat)
    text(y_title_pos$x, y_title_pos$y, labels = "Bit vector size (bits)",
         cex = 0.95, font = 2, col = INK, srt = y_ang)

    # Z axis (Memory Overhead %) along the outer-left vertical edge (x = x0, y = y1)
    zticks <- pretty(zlim, n = 6)
    zticks <- zticks[zticks >= z0 & zticks <= z1]
    z_ticks_start <- grDevices::trans3d(rep(x0, length(zticks)), rep(y1, length(zticks)), zticks, pmat)
    z_ticks_end   <- grDevices::trans3d(rep(x0 - 0.035 * dx_span, length(zticks)), rep(y1, length(zticks)), zticks, pmat)
    segments(z_ticks_start$x, z_ticks_start$y, z_ticks_end$x, z_ticks_end$y, col = INK, lwd = 0.8)
    z_lbl_pos <- grDevices::trans3d(rep(x0 - 0.06 * dx_span, length(zticks)), rep(y1, length(zticks)), zticks, pmat)
    text(z_lbl_pos$x, z_lbl_pos$y, labels = format_overhead(zticks), cex = 0.75, col = INK, adj = 1)
    z_title_pos <- grDevices::trans3d(x0 - 0.24 * dx_span, y1, mean(zlim), pmat)
    text(z_title_pos$x, z_title_pos$y, labels = "Overhead (%)", cex = 0.95, font = 2, col = INK, srt = 90)

    # Title & subtitle
    mtext(paste0(cfg$title, " — 3D Overhead Surface"), side = 3, line = 2.0,
          cex = 1.25, font = 2, col = NAVY, adj = 0)
    mtext(paste0("height (Z): memory overhead vs naive (%) | color: best structure (naive if none)",
                 if (cfg$shade) " | lighter = more ops needed" else ""),
          side = 3, line = 0.7, cex = 0.82, col = INK, adj = 0)

    # Legend cleanly inside the right margin
    usr <- par("usr")
    leg_names <- c("naive", g3$structs)
    legend(x = usr[2] + (usr[2] - usr[1]) * 0.03,
           y = mean(usr[3:4]), yjust = 0.5,
           title = "Best choice", title.col = INK, title.adj = 0,
           legend = leg_names,
           fill = unname(g3$pal[leg_names]),
           border = INK, bty = "n", cex = 0.95, text.col = INK)
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
    g   <- compute_smooth_grid(res, cfg)

    ggsave(cfg$output, build_smooth_plot(res, cfg, g), width = 10, height = 5.5, dpi = 300)
    cat(sprintf("Gráfico salvo com sucesso em: %s\n", cfg$output))

    if (nzchar(cfg$memory)) {
        mem <- load_memory(cfg$memory)
        if (nzchar(cfg$memory_contours)) {
            ggsave(cfg$memory_contours, build_contour_plot(res, cfg, g, mem),
                   width = 10, height = 5.5, dpi = 300)
            cat(sprintf("Gráfico com curvas de nível salvo em: %s\n", cfg$memory_contours))
        }
        if (nzchar(cfg$memory_output)) {
            ggsave(cfg$memory_output, build_memory_smooth_plot(res, cfg, g, mem),
                   width = 10, height = 5.5, dpi = 300)
            cat(sprintf("Gráfico contínuo de memória salvo em: %s\n", cfg$memory_output))
        }
        if (nzchar(cfg$memory_3d)) {
            save_3d_plot(res, cfg, mem, cfg$memory_3d)
            cat(sprintf("Gráfico 3D salvo em: %s\n", cfg$memory_3d))
        }
    }
}

if (sys.nframe() == 0L) main(commandArgs(trailingOnly = TRUE))
