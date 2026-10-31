FROM ubuntu:24.04

# Устанавливаем curl для HEALTHCHECK
RUN apt-get update && \
    apt-get install -y --no-install-recommends curl && \
    rm -rf /var/lib/apt/lists/*

# Копируем наш собранный deb-пакет
COPY dist/*.deb /tmp/app.deb
# Устанавливаем пакет (зависимости подтянутся автоматически) и удаляем его
RUN apt-get update && \
    apt-get install -y --no-install-recommends /tmp/app.deb && \
    rm -rf /var/lib/apt/lists/* /tmp/app.deb

# Запускаем от непривилегированного пользователя (числовой ID)
USER 10001

# Открываем порт
EXPOSE 8080

# Проверка здоровья контейнера
HEALTHCHECK --interval=30s --timeout=3s \
  CMD curl -fs http://localhost:8080/health || exit 1

# Команда запуска нашего сервера
CMD ["cryptofile", "serve"]
