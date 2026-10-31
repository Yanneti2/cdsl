suppressPackageStartupMessages({
  library(ggplot2)
  library(dplyr)
})

# Capturar os argumentos de execução
# Uso: Rscript resolver_x.R arquivo_jacobson.csv arquivo_bitvector.csv
args <- commandArgs(trailingOnly = TRUE)

if (length(args) != 4) {
  stop("Erro: Forneça exatamente os dois ficheiros como argumento.\nUso: Rscript resolver_x.R arquivo_jacobson.csv arquivo_bitvector.csv")
}

caminho_jacobson <- args[1]
caminho_bitvector <- args[2]
out_name_csv <- args[3]
out_name_graph <- args[4]

# Ler os ficheiros CSV
df_jacobson <- read.csv(caminho_jacobson, sep = ";", header = TRUE, dec = ".")
df_bitvector <- read.csv(caminho_bitvector, sep = ";", header = TRUE, dec = ".")

# Unir os dois dataframes através do campo "Size" em comum
# dados_unidos <- inner_join(df_jacobson, df_bitvector, by = "Size", suffix = c("_jac", "_bv"))

# # --- CÁLCULO DE X ---
# dados_unidos$x <- dados_unidos$RS_Time / (dados_unidos$Time_bv - dados_unidos$Time_jac)

# Agrupar os dados por Size
jac <- df_jacobson %>%
  group_by(Size) %>%
  summarise(
    Time_jac = mean(Time),
    RS_Time = mean(BS_Time), # RS_TIME for jacob and rrr
    .groups = "drop"
  )

bv <- df_bitvector %>%
  group_by(Size) %>%
  summarise(
    Time_bv = mean(Time),
    .groups = "drop"
  )

# Unir os dados agrupados
dados_unidos <- inner_join(
  jac,
  bv,
  by = "Size"
) %>%
  mutate(
    x = RS_Time / (Time_bv - Time_jac)
  )

# Ordenar por Size e identificar o valor anterior
dados_unidos <- dados_unidos %>%
  arrange(Size) %>%
  mutate(
    Size_ant = lag(Size),
    x_ant = lag(x)
  )

# Encontrar todas as mudanças de sinal
transicoes <- dados_unidos %>%
  filter(
    !is.na(x_ant),
    x_ant * x < 0
  ) %>%
  mutate(
    size_mid = sqrt(Size_ant * Size)
  )

# --- SELECIONAR APENAS SIZE E X PARA O CSV ---
dados_csv <- dados_unidos %>% select(Size, x)

# output_csv <- "resultado_x.csv"
output_csv <- out_name_csv
write.csv2(dados_csv, output_csv, row.names = FALSE)
cat(paste("Ficheiro CSV gerado apenas com Size e x e guardado em:", output_csv, "\n"))

# --- GERAR E GUARDAR O GRÁFICO ---
# grafico_x <- ggplot(dados_unidos, aes(x = Size, y = x)) +
#   geom_line(color = "#2b5c8f", linewidth = 1.2) +
#   geom_point(color = "#1f4068", size = 3) +
#   labs(
#     title = "Valor de x em função do Tamanho (Size)",
#     subtitle = "Cálculo baseado nos campos TIME do Jacobson e Bitvector",
#     x = "Tamanho (Size)",
#     y = "Valor de x"
#   ) +
#   theme_minimal(base_size = 13) +
#   theme(
#     plot.title = element_text(face = "bold", hjust = 0.5),
#     plot.subtitle = element_text(hjust = 0.5, color = "gray40")
#   )

grafico_x <- ggplot(dados_unidos, aes(x = Size, y = x)) +

  # Destacar os intervalos de mudança de sinal
  geom_rect(
    data = transicoes,
    aes(
      xmin = Size_ant,
      xmax = Size,
      ymin = -Inf,
      ymax = Inf
    ),
    inherit.aes = FALSE,
    fill = "red",
    alpha = 0.15
  ) +

  # Linha dos valores
  geom_line(color = "#2b5c8f", linewidth = 0.8) +

  # Pontos
  geom_point(color = "#1f4068", size = 1.5) +

  # Linha vertical na transição
  # geom_vline(
  #   data = transicoes,
  #   aes(xintercept = size_mid),
  #   color = "red",
  #   linetype = "dotted",
  #   linewidth = 1
  # ) +

  # IDENTIFICAÇÃO DO INTERVALO
  geom_label(
    data = transicoes,
    aes(
      x = size_mid,
      y = 0,
      label = paste0(
        "Size: ", Size_ant, " -> ", Size
      )
    ),
    inherit.aes = FALSE,
    color = "red",
    fill = "white",
    fontface = "bold",
    size = 3.5,
    nudge_y = 2
  ) +

  # ESCALA LOGARÍTMICA X COM MARCAÇÕES PERSONALIZADAS
  scale_x_log10(
    breaks = function(lim) {
      sort(unique(c(
        scales::breaks_log(n = 10)(lim),
        transicoes$Size_ant,
        transicoes$Size
      )))
    },
    labels = scales::label_number(
      big.mark = ".",
      decimal.mark = ","
    )
  ) +

  # ESCALA Y PARA NEGATIVOS E POSITIVOS
  scale_y_continuous(
    trans = scales::pseudo_log_trans(sigma = 1)
  ) +

  labs(
    title = "Valor de x em função do Tamanho",
    subtitle = "Transições de sinal destacadas em vermelho",
    x = "Size (escala logarítmica)",
    y = "Valor de x (escala pseudo-logarítmica)"
  ) +

  theme_minimal(base_size = 13) +
  theme(
    plot.title = element_text(face = "bold", hjust = 0.5),
    plot.subtitle = element_text(hjust = 0.5),
    axis.text.x = element_text(angle = 45, hjust = 1)
  )

output_grafico <- out_name_graph
ggsave(output_grafico, plot = grafico_x, width = 9, height = 5, dpi = 300)
cat(paste("Gráfico gerado com sucesso e guardado em:", output_grafico, "\n"))