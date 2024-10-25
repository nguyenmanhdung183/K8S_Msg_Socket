#!/bin/bash

# Kiểm tra xem chương trình client đã được biên dịch hay chưa
CLIENT_EXEC="./client"
if [ ! -f "$CLIENT_EXEC" ]; then
  echo "Client executable not found. Please compile your client code first."
  exit 1
fi

# Chạy 10 client với các ID khác nhau, mỗi client trong một terminal riêng
for id in {1..10}; do
  echo "Starting client with ID $id in a new terminal..."
  # Mở Xterm và chạy client với ID tương ứng
  xterm -hold -e "$CLIENT_EXEC $id" &
done

echo "All clients started."
