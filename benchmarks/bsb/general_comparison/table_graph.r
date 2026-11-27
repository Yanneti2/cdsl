#!/usr/bin/env Rscript
#
#
# Uso:
#   Rscript table_graph.r {Comparacao.csv} {Nome} [{Comparacao.csv} {Nome} ...] [flags]
#
#
# Flags:
#   --order a,b,c         estruturas, da mais "fria" à mais "quente" (default: bs,jacobson,rrr)
#   --max-size N          tamanho máximo (default: 10000)
#   --sizes a,b,c         tamanhos alvo (default: 64,128,256,512,1024,2048,4096,8192)
#   --densities a,b,c     densidades (default: .1,1,5,10,30,50,70,90,95,99,99.9)
#   --x-axis COL          coluna do tamanho (default: Size)
#   --value-col COL       coluna do break-even (default: x, senão a 2ª coluna)
#   --density-col COL     coluna da densidade (default: Density)
#   --title/-t TXT        título
#   --output/-o ARQ       PNG do heatmap (default: breakeven.png)
#   --table-out ARQ       tabela de saída .xlsx (default) ou .csv
#   --memory ARQ          CSV com overhead de memória (Structure;Size;Density;Overhead)
#   --memory-output ARQ   PNG do heatmap separado de memória (default: memory.png)
#   --memory-combined ARQ PNG do heatmap combinado (operações + overhead na mesma célula)
#   --memory-in-cell      inclui overhead abaixo do nº de operações no gráfico principal (-o)

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
        output = "breakeven.png",
        max_size = 10000,
        sizes = c(64, 128, 256, 512, 1024, 2048, 4096, 8192),
        densities = c(.1, 1, 5, 10, 30, 50, 70, 90, 95, 99, 99.9),
        table_out = "breakeven_table.xlsx",
        memory = "", memory_output = "memory.png",
        memory_combined = "", memory_in_cell = FALSE
    )
    flags <- list(
        "--order" = "order", "--max-size" = "max_size", "--sizes" = "sizes",
        "--densities" = "densities", "--x-axis" = "x_axis",
        "--value-col" = "value_col", "--density-col" = "density_col",
        "--title" = "title", "-t" = "title",
        "--output" = "output", "-o" = "output", "--table-out" = "table_out",
        "--memory" = "memory", "--memory-output" = "memory_output",
        "--memory-combined" = "memory_combined"
    )
    nums <- function(s) as.numeric(trimws(strsplit(s, ",")[[1]]))
    i <- 1
    while (i <= length(args)) {
        a <- args[i]
        if (a == "--memory-in-cell") {
            cfg$memory_in_cell <- TRUE
        } else if (!is.null(flags[[a]])) {
            if (i >= length(args)) stop(paste(a, "requer um valor"))
            i <- i + 1
            key <- flags[[a]]
            cfg[[key]] <- switch(key,
                order     = trimws(strsplit(args[i], ",")[[1]]),
                sizes     = nums(args[i]),
                densities = nums(args[i]),
                max_size  = as.numeric(args[i]),
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
        rep(as.numeric(sub("[-_]d", "", m)), nrow(raw))   # "d.1" -> 0.1, "d99.9" -> 99.9
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

# --------------------------------------------------------------- results ----

# Grade completa (estrutura x tamanho x densidade); o que não tem dado vira no_data.
build_results <- function(df, cfg) {
    grid <- expand.grid(
        Structure = unique(df$Structure), Size = attr(df, "sizes"),
        Density = round(cfg$densities, 3), stringsAsFactors = FALSE
    )
    grid %>%
        left_join(df, by = c("Structure", "Size", "Density")) %>%
        mutate(
            status = case_when(
                is.na(x) ~ "no_data",
                x >= 0   ~ "pays_off",
                TRUE     ~ "never"
            ),
            n = ifelse(status == "pays_off", pmax(ceiling(x), 1), NA_real_)
        )
}

build_table <- function(res, cfg) {
    lv <- c(cfg$order, setdiff(unique(res$Structure), cfg$order))
    res %>%
        mutate(Structure = factor(Structure, levels = lv)) %>%
        arrange(Size, Density, Structure) %>%
        transmute(
            Structure_name = as.character(Structure),
            size           = Size,
            density        = Density,
            x              = x,
            a_partir_de_quantos_x_compensa_construir = n,
            status         = status
        ) %>%
        as.data.frame()
}

write_table <- function(tab, path) {
    if (grepl("\\.csv$", path, ignore.case = TRUE)) {
        write.table(tab, path, sep = ";", dec = ",", row.names = FALSE, quote = FALSE, na = "")
    } else {
        if (!requireNamespace("writexl", quietly = TRUE)) {
            stop("Instale o pacote writexl: install.packages(\"writexl\")  (ou use --table-out arquivo.csv)")
        }
        writexl::write_xlsx(list(breakeven = tab), path)
    }
}

# ------------------------------------------------------------------ plot ----

format_n <- function(n) {
    ifelse(n >= 1e6, sprintf("%.1fM", n / 1e6),
    ifelse(n >= 1e4, sprintf("%.0fk", n / 1e3),
    ifelse(n >= 1e3, sprintf("%.1fk", n / 1e3), sprintf("%.0f", n))))
}

STRUCT_COLORS <- c(bitvectorBS = "#F6921E", bs = "#F6921E",
                   jacobson = "#112A52", rrr = "#8C4A12")
EXTRA_COLORS  <- c("#5B7DB1", "#8C4A12", "#FBC47F")
NAVY <- "#112A52"
INK  <- "#29323D"

make_palette <- function(present, cfg) {
    lv <- c(cfg$order[cfg$order %in% present], setdiff(present, cfg$order))
    pal <- setNames(rep(NA_character_, length(lv)), lv)
    known <- intersect(lv, names(STRUCT_COLORS))
    pal[known] <- STRUCT_COLORS[known]
    other <- names(pal)[is.na(pal)]
    if (length(other) > 0) pal[other] <- rep(EXTRA_COLORS, length.out = length(other))
    list(lv = lv, pal = c(naive = "#38636B", pal, none = "#BDBDBD"))
}

build_plot <- function(res, cfg) {
    win <- res %>%
        filter(status == "pays_off") %>%
        group_by(Size, Density) %>%
        slice(which.min(n)) %>%            # menor nº de operações
        ungroup() %>%
        select(Size, Density, Winner = Structure, n)

    has_data <- res %>%
        group_by(Size, Density) %>%
        summarise(any_data = any(status != "no_data"), .groups = "drop")

    cells <- has_data %>%
        left_join(win, by = c("Size", "Density")) %>%
        mutate(
            Structure = case_when(
                !is.na(Winner) ~ Winner,
                any_data       ~ "naive",
                TRUE           ~ "none"
            ),
            label = case_when(
                !is.na(n)          ~ format_n(n),
                Structure == "none" ~ "n/a",
                TRUE               ~ "\u2014"
            )
        )

    pp  <- make_palette(unique(res$Structure), cfg)
    lv  <- pp$lv
    pal <- pp$pal

    cells$Structure <- factor(cells$Structure, levels = c("naive", lv))
    sz <- sort(unique(cells$Size), decreasing = TRUE)
    ds <- sort(unique(cells$Density))
    cells$Size_f <- factor(format_n(cells$Size), levels = format_n(rev(sz)))
    cells$Dens_f <- factor(cells$Density, levels = ds,
                           labels = format(ds, trim = TRUE, drop0trailing = TRUE))

    rgbm <- col2rgb(pal[as.character(cells$Structure)])
    lum  <- colSums(rgbm * c(0.299, 0.587, 0.114))
    text_color <- ifelse(lum < 140, "white", INK)

    ggplot(cells, aes(x = Dens_f, y = Size_f, fill = Structure)) +
        geom_tile(color = "white", linewidth = 0.8, show.legend = TRUE) +
        geom_text(aes(label = label), size = 3.5, fontface = "bold",
                  color = text_color) +
        scale_fill_manual(values = pal, drop = FALSE) +
        labs(
             title    = cfg$title,
             subtitle = "color: structure that pays off with the fewest operations (naive if none does) | number: operations needed",
             x = "Density (%)", y = "Bit vector size (bits)", fill = "Best choice"
             ) +
        theme_minimal(base_size = 12) +
        theme(text = element_text(color = INK),
              axis.text = element_text(color = INK),
              panel.grid = element_blank(),
              plot.title = element_text(color = NAVY, face = "bold", size = 12),
              plot.subtitle = element_text(size = 8))
}

# ------------------------------------------------------------------ main ----

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

format_overhead <- function(v) {
    ifelse(is.na(v), "n/a",
    ifelse(round(v) == 0, "0%",
           sprintf("%+.0f%%", v)))
}

build_combined_plot <- function(res, cfg) {
    base_p <- build_plot(res, cfg)
    mem    <- load_memory(cfg$memory)

    cells <- base_p$data %>%
        mutate(Structure_chr = as.character(Structure)) %>%
        left_join(mem, by = c("Structure_chr" = "Structure", "Size", "Density")) %>%
        mutate(
            Overhead  = ifelse(Structure_chr == "naive", 0, Overhead),
            mem_label = format_overhead(Overhead)
        )

    pp  <- make_palette(unique(res$Structure), cfg)
    pal <- pp$pal

    rgbm <- col2rgb(pal[cells$Structure_chr])
    lum  <- colSums(rgbm * c(0.299, 0.587, 0.114))
    text_color <- ifelse(lum < 140, "white", INK)

    ggplot(cells, aes(x = Dens_f, y = Size_f, fill = Structure)) +
        geom_tile(color = "white", linewidth = 0.8, show.legend = TRUE) +
        geom_text(aes(label = label), size = 3.3, fontface = "bold",
                  color = text_color, vjust = -0.15) +
        geom_text(aes(label = mem_label), size = 2.4, fontface = "plain",
                  color = text_color, alpha = 0.85, vjust = 1.35) +
        scale_fill_manual(values = pal, drop = FALSE) +
        labs(
            title    = cfg$title,
            subtitle = "color: best structure (naive if none) | top: operations needed | bottom: memory overhead vs naive",
            x = "Density (%)", y = "Bit vector size (bits)", fill = "Best choice"
        ) +
        theme_minimal(base_size = 12) +
        theme(text = element_text(color = INK),
              axis.text = element_text(color = INK),
              panel.grid = element_blank(),
              plot.title = element_text(color = NAVY, face = "bold", size = 12),
              plot.subtitle = element_text(size = 8))
}

build_memory_plot <- function(res, cfg) {
    mem <- load_memory(cfg$memory)

    # same cells and winners as the first heatmap
    cells <- build_plot(res, cfg)$data %>%
        mutate(Structure = as.character(Structure)) %>%
        left_join(mem, by = c("Structure", "Size", "Density")) %>%
        mutate(Overhead = ifelse(Structure == "naive", 0, Overhead),
               label = format_overhead(Overhead))

    rng  <- range(cells$Overhead, na.rm = TRUE)
    high <- !is.na(cells$Overhead) & cells$Overhead > mean(rng)

    ggplot(cells, aes(x = Dens_f, y = Size_f, fill = Overhead)) +
        geom_tile(color = "white", linewidth = 0.8) +
        geom_text(aes(label = label), size = 3.5, fontface = "bold",
                  color = ifelse(high, "white", "black")) +
        scale_fill_gradient(low = "#F2F2F2", high = "#112A52",
                            na.value = "grey70") +
        labs(title = paste0(cfg$title, " — Memory Overhead"),
             x = "Density (%)", y = "Bit vector size (bits)",
             fill = "Overhead (%)") +
        theme_minimal(base_size = 12) +
        theme(panel.grid = element_blank(),
              plot.title = element_text(color = NAVY, face = "bold", size = 12))
}

main <- function(args) {
    cfg <- parse_args(args)
    df  <- load_all(cfg)

    # Densidades nos arquivos vs. esperadas: nada some em silêncio.
    expected <- round(cfg$densities, 3)
    found    <- sort(unique(df$Density))
    if (length(setdiff(found, expected)) > 0)
        cat("Aviso: densidades nos arquivos fora de --densities (ignoradas):",
            paste(setdiff(found, expected), collapse = ", "), "\n")
    if (length(setdiff(expected, found)) > 0)
        cat("Aviso: densidades SEM nenhum arquivo (colunas n/a):",
            paste(setdiff(expected, found), collapse = ", "), "\n")

    res <- build_results(df, cfg)

    tab <- build_table(res, cfg)
    write_table(tab, cfg$table_out)
    cat(sprintf("Tabela salva em: %s (%d linhas)\n", cfg$table_out, nrow(tab)))

    nd <- sum(res$status == "no_data")
    if (nd > 0) cat(sprintf("Aviso: %d combinações sem dados (status = no_data).\n", nd))

    main_plot <- if (cfg$memory_in_cell && nzchar(cfg$memory)) {
        build_combined_plot(res, cfg)
    } else {
        build_plot(res, cfg)
    }
    ggsave(cfg$output, main_plot, width = 10, height = 5.5, dpi = 300)
    cat(sprintf("Gráfico salvo com sucesso em: %s\n", cfg$output))

    if (nzchar(cfg$memory)) {
        if (nzchar(cfg$memory_output)) {
            ggsave(cfg$memory_output, build_memory_plot(res, cfg),
                   width = 10, height = 5.5, dpi = 300)
            cat(sprintf("Gráfico da memoria salvo com sucesso em: %s\n", cfg$memory_output))
        }
        if (nzchar(cfg$memory_combined)) {
            ggsave(cfg$memory_combined, build_combined_plot(res, cfg),
                   width = 10, height = 5.5, dpi = 300)
            cat(sprintf("Gráfico combinado salvo com sucesso em: %s\n", cfg$memory_combined))
        }
    }
}

if (!interactive()) main(commandArgs(trailingOnly = TRUE))
