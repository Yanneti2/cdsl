suppressPackageStartupMessages({
  library(ggplot2)
  library(dplyr)
})

# Capturar os argumentos de execução
# Uso: Rscript resolver_x.R arquivo_jacobson.csv arquivo_bitvector.csv
args <- commandArgs(trailingOnly = TRUE)

if (length(args) < 2) {
  stop("Erro: Forneça exatamente os dois ficheiros como argumento.\nUso: Rscript resolver_x.R arquivo_jacobson.csv arquivo_bitvector.csv")
}

caminho_jacobson <- args[1]
caminho_bitvector <- args[2]

# Ler os ficheiros CSV
df_jacobson <- read.csv(caminho_jacobson, sep = ";", header = TRUE, dec = ".")
df_bitvector <- read.csv(caminho_bitvector, sep = ";", header = TRUE, dec = ".")

# Unir os dois dataframes através do campo "Size" em comum
dados_unidos <- inner_join(df_jacobson, df_bitvector, by = "Size", suffix = c("_jac", "_bv"))

# --- CÁLCULO DE X ---
dados_unidos$x <- dados_unidos$RS_Time / (dados_unidos$Time_bv - dados_unidos$Time_jac)

# --- SELECIONAR APENAS SIZE E X PARA O CSV ---
dados_csv <- dados_unidos %>% select(Size, x)

output_csv <- "resultado_x.csv"
write.csv2(dados_csv, output_csv, row.names = FALSE)
cat(paste("Ficheiro CSV gerado apenas com Size e x e guardado em:", output_csv, "\n"))

# --- GERAR E GUARDAR O GRÁFICO ---
grafico_x <- ggplot(dados_unidos, aes(x = Size, y = x)) +
  geom_line(color = "#2b5c8f", linewidth = 1.2) +
  geom_point(color = "#1f4068", size = 3) +
  labs(
    title = "Valor de x em função do Tamanho (Size)",
    subtitle = "Cálculo baseado nos campos TIME do Jacobson e Bitvector",
    x = "Tamanho (Size)",
    y = "Valor de x"
  ) +
  theme_minimal(base_size = 13) +
  theme(
    plot.title = element_text(face = "bold", hjust = 0.5),
    plot.subtitle = element_text(hjust = 0.5, color = "gray40")
  )

output_grafico <- "resultado_x.png"
ggsave(output_grafico, plot = grafico_x, width = 9, height = 5, dpi = 300)
cat(paste("Gráfico gerado com sucesso e guardado em:", output_grafico, "\n"))