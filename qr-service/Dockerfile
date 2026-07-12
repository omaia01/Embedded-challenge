FROM debian:12

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake

WORKDIR /app

COPY . .

RUN mkdir -p build

WORKDIR /app/build

RUN cmake ..

RUN make

CMD ["./qr-service"]