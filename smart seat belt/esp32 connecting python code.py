import requests

# ESP32 IP address (you can get it from the Serial Monitor of the ESP32)
esp32_ip = "http://192.168.37.254"  # Replace with your ESP32's IP address

# Function to set GPIO13 HIGH
def set_gpio_high():
    url = f"{esp32_ip}/set_gpio"
    response = requests.get(url)
    print(response.text)
    

# Function to set GPIO13 LOW
def set_gpio_low():
    url = f"{esp32_ip}/reset_gpio"
    response = requests.get(url)
    print(response.text)

# Example usage
command = input("Enter command (HIGH/LOW): ")

if command.upper() == "HIGH":
    set_gpio_high()
elif command.upper() == "LOW":
    set_gpio_low()
else:
    print("Invalid command. Please enter 'HIGH' or 'LOW'.")
