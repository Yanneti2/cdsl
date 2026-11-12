#!/usr/bin/env Rscript
#
# Break-even analysis of rank/select structures vs. a naive bit vector.
#
# For every (size, density), a structure S pays off after n operations when
#     C_S / n + T_S < T_naive      <=>      n > C_S / (T_naive - T_S)
# (C_S = construction time, T_S = time per operation). The smallest such
# integer n is reported; if T_S >= T_naive the structure never pays off.
#
# Uso:
#   Rscript breakeven_heatmap.R {Dataset.csv} {Nome} [{Dataset.csv} {Nome} ...] [flags]
#
# Um CSV por (estrutura, densidade), repetindo o mesmo Nome. A densidade vem da
# coluna "Density" ou do nome do arquivo ("-d30", "-d0.1"). A operação vem de
# --operation ou do nome do arquivo ("-r1-", "-s0-" ...).
#
# Flags:
#   --baseline NOME      estrutura de referência (default: naive)
#   --order a,b,c        da mais "fria" à mais "quente" (default: bs,jacobson,rrr)
#   --max-size N         tamanho máximo comparado (default: 10000)
#   --sizes a,b,c        tamanhos alvo (default: 128,256,...,8192,10000)
#   --densities a,b,c    densidades esperadas (default: 0.1,1,5,10,30,50,70,90,95,99,99.9)
#   --operation TXT      rótulo da operação (default: deduzido do arquivo)
#   --table-out ARQ      tabela de saída: .xlsx (default) ou .csv
#   --x-axis COL         coluna do tamanho (default: Size)
#   --density-col COL    coluna da densidade (default: Density)
#   --title/-t TXT       título do heatmap
#   --output/-o ARQ      PNG do heatmap (default: breakeven.png)

suppressPackageStartupMessages({
    library(ggplot2)
    library(dplyr)
})

# ---------------------------------------------------------------- args ----

parse_args <- function(args) {
    cfg <- list(
        datasets = character(), names = character(),
        baseline = "naive", order = c("bs", "jacobson", "rrr"),
        x_axis = "Size", density_col = "Density",
        title = "Choosing Rank and Select Structures on Low Order Bit Vectors: Break-even Operation Count",
        output = "breakeven.png",
        max_size = 10000,
        sizes = c(128, 256, 512, 1024, 2048, 4096, 8192, 10000),
        densities = c(0.1, 1, 5, 10, 30, 50, 70, 90, 95, 99, 99.9),
        operation = NULL,
        table_out = "breakeven_table.xlsx"
    )
    flags <- list(
        "--baseline" = "baseline", "--order" = "order",
        "--max-size" = "max_size", "--sizes" = "sizes", "--densities" = "densities",
        "--operation" = "operation", "--table-out" = "table_out",
        "--x-axis" = "x_axis", "--density-col" = "density_col",
        "--title" = "title", "-t" = "title",
        "--output" = "output", "-o" = "output"
    )
    nums <- function(s) as.numeric(trimws(strsplit(s, ",")[[1]]))
    i <- 1
    while (i <= length(args)) {
        a <- args[i]
        if (!is.null(flags[[a]])) {
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

sum_cols <- function(raw, cols) {
    cols <- intersect(cols, names(raw))
    if (length(cols) == 0) return(rep(0, nrow(raw)))
    rowSums(sapply(raw[cols], function(col) {
        v <- as.numeric(col)
        ifelse(is.na(v) | v == -1, 0, v)
    }))
}

guess_operation <- function(path) {
    b <- basename(path)
    m <- regmatches(b, regexpr("(^|[-_])[rs][01](?=[-_.])", b, perl = TRUE))
    if (length(m) == 0) "op" else sub("^[-_]", "", m)
}

load_one <- function(path, name, cfg) {
    raw <- read.csv(path, sep = ";")
    if (!cfg$x_axis %in% names(raw)) stop(sprintf("%s: coluna '%s' não encontrada.", path, cfg$x_axis))

    dens <- if (cfg$density_col %in% names(raw)) {
        as.numeric(raw[[cfg$density_col]])
    } else {
        m <- regmatches(basename(path), regexpr("[-_]d[0-9]+(\\.[0-9]+)?", basename(path)))
        if (length(m) == 0) stop(sprintf("%s: densidade não encontrada (coluna ou '-dNN' no nome).", path))
        rep(as.numeric(sub("[-_]d", "", m)), nrow(raw))
    }

    data.frame(
        Size       = as.numeric(raw[[cfg$x_axis]]),
        Density    = round(dens, 3),
        Op_Time    = as.numeric(raw[["Time"]]),
        Const_Time = sum_cols(raw, c("BV_Time", "RS_Time", "SS_Time")),
        Total_Mem  = sum_cols(raw, c("BV_Memory", "RS_Memory", "SS_Memory")),
        Structure  = name,
        Operation  = if (is.null(cfg$operation)) guess_operation(path) else cfg$operation,
        stringsAsFactors = FALSE
    )
}

# Escolhe os tamanhos-alvo dentre os existentes (mais próximo em escala log).
select_sizes <- function(sizes, cfg) {
    u <- sort(unique(sizes[is.finite(sizes) & sizes > 0 & sizes <= cfg$max_size]))
    if (length(u) == 0) stop("Nenhum tamanho <= --max-size nos datasets.")
    tg <- cfg$sizes[cfg$sizes <= cfg$max_size]
    unique(sapply(tg, function(s) u[which.min(abs(log(u) - log(s)))]))
}

# Média apenas das repetições de uma mesma (estrutura, operação, tamanho, densidade).
load_all <- function(cfg) {
    all  <- bind_rows(lapply(seq_along(cfg$datasets), function(i) load_one(cfg$datasets[i], cfg$names[i], cfg)))
    keep <- select_sizes(all$Size, cfg)
    df <- all %>%
        filter(Size %in% keep) %>%
        group_by(Structure, Operation, Size, Density) %>%
        summarise(Op_Time = mean(Op_Time, na.rm = TRUE),
                  Const_Time = mean(Const_Time, na.rm = TRUE),
                  Total_Mem = mean(Total_Mem, na.rm = TRUE), .groups = "drop")
    attr(df, "sizes") <- keep
    df
}

# ------------------------------------------------------------ break-even ----

# Grade completa (estrutura x operação x tamanho x densidade). Combinações sem
# dados ficam com NA e status "no_data".
build_results <- function(df, cfg) {
    grid <- expand.grid(
        Structure = unique(df$Structure), Operation = unique(df$Operation),
        Size = attr(df, "sizes"), Density = round(cfg$densities, 3),
        stringsAsFactors = FALSE
    )
    base <- df %>%
        filter(Structure == cfg$baseline) %>%
        select(Operation, Size, Density, Base_Time = Op_Time)
    if (nrow(base) == 0) stop("Baseline '", cfg$baseline, "' não encontrado nos datasets.")

    grid %>%
        left_join(df, by = c("Structure", "Operation", "Size", "Density")) %>%
        left_join(base, by = c("Operation", "Size", "Density")) %>%
        mutate(
            gain = Base_Time - Op_Time,
            status = case_when(
                Structure == cfg$baseline                                   ~ ifelse(is.na(Op_Time), "no_data", "baseline"),
                is.na(Op_Time) | is.na(Const_Time) | is.na(Base_Time)       ~ "no_data",
                gain <= 0                                                   ~ "never",
                TRUE                                                        ~ "pays_off"
            ),
            # menor n inteiro com C/n + T_s < T_naive  (desigualdade estrita)
            n = ifelse(status == "pays_off", floor(Const_Time / gain) + 1, NA_real_)
        )
}

build_table <- function(res, cfg) {
    lv <- c(cfg$baseline, setdiff(cfg$order, cfg$baseline),
            setdiff(unique(res$Structure), c(cfg$order, cfg$baseline)))
    res %>%
        mutate(Structure = factor(Structure, levels = lv)) %>%
        arrange(Operation, Size, Density, Structure) %>%
        transmute(
            Structure_name = as.character(Structure),
            operation      = Operation,
            size           = Size,
            density        = Density,
            avg_time       = round(Op_Time, 2),
            avg_mem        = round(Total_Mem, 2),
            a_partir_de_quantos_x_compensa_construir = n,
            status         = status
        ) %>%
        as.data.frame()
}

write_table <- function(tab, path) {
    if (grepl("\\.csv$", path, ignore.case = TRUE)) {
        write.table(tab, path, sep = ";", row.names = FALSE, quote = FALSE, na = "")
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

build_plot <- function(res, cfg) {
    win <- res %>%
        filter(status == "pays_off") %>%
        group_by(Size, Density) %>%
        slice(which.min(n)) %>%          # estrutura com MENOR nº de operações
        ungroup() %>%
        select(Size, Density, Winner = Structure, n)

    has_base <- res %>%
        filter(Structure == cfg$baseline, status == "baseline") %>%
        distinct(Size, Density) %>%
        mutate(has_base = TRUE)

    cells <- res %>%
        distinct(Size, Density) %>%
        left_join(win, by = c("Size", "Density")) %>%
        left_join(has_base, by = c("Size", "Density")) %>%
        mutate(
            Structure = case_when(
                !is.na(Winner)  ~ Winner,
                !is.na(has_base) ~ cfg$baseline,
                TRUE            ~ "no data"
            ),
            label = case_when(
                !is.na(n)        ~ format_n(n),
                Structure == "no data" ~ "n/a",
                TRUE             ~ "\u2014"
            )
        )

    # frio -> quente na ordem pedida; baseline cinza
    present <- setdiff(unique(res$Structure), cfg$baseline)
    lv  <- cfg$order[cfg$order %in% present]
    lv  <- c(lv, setdiff(present, lv))
    pal <- setNames(colorRampPalette(c("#f1c40f", "#e67e22", "#d32f2f"))(length(lv)), lv)
    pal[cfg$baseline] <- "#bdbdbd"
    pal["no data"]    <- "#f5f5f5"

    cells$Structure <- factor(cells$Structure, levels = c(lv, cfg$baseline, "no data"))
    sz <- sort(unique(cells$Size))
    ds <- sort(unique(cells$Density))
    cells$Size_f <- factor(format_n(cells$Size), levels = format_n(rev(sz)))
    cells$Dens_f <- factor(cells$Density, levels = ds,
                           labels = format(ds, trim = TRUE, drop0trailing = TRUE))

    ggplot(cells, aes(x = Dens_f, y = Size_f, fill = Structure)) +
        geom_tile(color = "white", linewidth = 0.8) +
        geom_text(aes(label = label), size = 3.5, fontface = "bold") +
        scale_fill_manual(values = pal, drop = FALSE,
                          labels = function(x) ifelse(x == cfg$baseline,
                                                      paste0(x, " (no structure pays off)"), x)) +
        labs(
            title    = cfg$title,
            subtitle = sprintf("Baseline: %s | color: structure that amortizes construction with the fewest operations | number: operations needed",
                               cfg$baseline),
            x = "Density (%)", y = "Bit vector size (bits)", fill = "Structure"
        ) +
        theme_minimal(base_size = 12) +
        theme(panel.grid = element_blank(),
              plot.title = element_text(face = "bold", size = 12),
              plot.subtitle = element_text(size = 8))
}

# ------------------------------------------------------------------ main ----

main <- function(args) {
    cfg <- parse_args(args)
    df  <- load_all(cfg)
    res <- build_results(df, cfg)

    tab <- build_table(res, cfg)
    write_table(tab, cfg$table_out)
    cat(sprintf("Tabela salva em: %s (%d linhas)\n", cfg$table_out, nrow(tab)))

    nd <- res %>% filter(status == "no_data") %>% nrow()
    if (nd > 0) cat(sprintf("Aviso: %d combinações sem dados (status = no_data).\n", nd))

    ops <- unique(df$Operation)
    if (length(ops) > 1) {
        warning("Mais de uma operação detectada (", paste(ops, collapse = ", "),
                "): heatmap não gerado. Rode uma vez por operação ou use --operation.")
    } else {
        ggsave(cfg$output, build_plot(res, cfg), width = 10, height = 5.5, dpi = 300)
        cat(sprintf("Gráfico salvo com sucesso em: %s\n", cfg$output))
    }
}

if (!interactive()) main(commandArgs(trailingOnly = TRUE))