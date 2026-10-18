# Build stage
FROM alpine:3.19 AS builder
RUN apk add --no-cache build-base
WORKDIR /app
COPY src/ src/
COPY tests/ tests/
COPY Makefile .
RUN make

# Runtime stage — tiny final image
FROM alpine:3.19
RUN apk add --no-cache libgcc
COPY --from=builder /app/sezar /usr/local/bin/sezar
ENTRYPOINT ["sezar"]
CMD ["--help"]
