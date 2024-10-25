# # Dockerfile

# # Sử dụng image cơ bản cho C++
# FROM gcc:11

# # Đặt thư mục làm việc
# WORKDIR /usr/src/app

# # Sao chép mã nguồn vào container
# COPY . .

# # Biên dịch mã nguồn
# RUN g++ -o server server.cpp -lpthread

# # Cổng mà server sẽ lắng nghe
# EXPOSE 8080

# # Chạy server
# CMD ["./server"]

# Sử dụng Alpine làm base image
FROM alpine:latest

# Cài đặt g++ và các thư viện cần thiết
RUN apk add --no-cache g++

# Thiết lập thư mục làm việc
WORKDIR /app

# Sao chép file C++ vào trong container
COPY server.cpp .

# Biên dịch chương trình C++
RUN g++ -o server server.cpp -pthread

# Lệnh để chạy chương trình đã biên dịch
CMD ["./server"]
