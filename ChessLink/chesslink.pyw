import serial
import time

baud_rate = 9600
handshake_message = "LINK-LOGON"
expected_response = "LOGON-CONFIRM"
max_duration = 60  # 1 minute in seconds
retry_interval = 3  # Retry every 3 seconds

def connect_to_arduino():
    port = 'COM3'  # Directly specify COM3
    start_time = time.time()  # Track the start time for 1-minute window

    try:
        print(f"Trying to connect to {port}...")
        arduino = serial.Serial(port, baud_rate, timeout=2)
        time.sleep(2)  # Wait for connection to stabilize

        while time.time() - start_time < max_duration:
            print(f"Sending handshake '{handshake_message}' to {port}")
            arduino.write(f"{handshake_message}\n".encode('utf-8'))
            time.sleep(2)  # Wait for the Arduino to process and respond

            # Check if data is available
            if arduino.in_waiting > 0:
                response = arduino.readline().decode('utf-8').strip()
                print(f"Received response: '{response}'")
                if response == expected_response:
                    print(f"Arduino successfully connected on {port}")
                    return arduino
                else:
                    print(f"Unexpected response: {response}")
            else:
                print("No response from Arduino.")
            
            # Wait before retrying
            time.sleep(retry_interval)
        
        print(f"Failed to connect within {max_duration} seconds.")
        arduino.close()
    except (serial.SerialException, OSError) as e:
        print(f"Error connecting to {port}: {e}")
        return None

def send_command_and_receive_data(command, arduino):
    """
    Send a command to Arduino and receive the response (data).
    """
    arduino.write(f"{command}\n".encode('utf-8'))  # Send the command
    time.sleep(1)  # Wait for Arduino to process and send data back

    if arduino.in_waiting > 0:
        response = arduino.readline().decode('utf-8').strip()
        print(f"Data received from Arduino: {response}")
        return response
    else:
        print("No data received.")
        return None

# Example usage
arduino = connect_to_arduino()

if arduino:
    # Fetch data (player times, pause status)
    data = send_command_and_receive_data("FETCH", arduino)
    print(f"Fetched data from Arduino: {data}")
    
    # Send a PUT command to change PLAYER1TIME
    player1_new_time = 150  # New time in seconds
    response = send_command_and_receive_data(f"PLAYER1TIME {player1_new_time}", arduino)
    print(f"Response from Arduino: {response}")
    
    # Send a PUT command to change PLAYER2TIME
    player2_new_time = 180  # New time in seconds
    response = send_command_and_receive_data(f"PLAYER2TIME {player2_new_time}", arduino)
    print(f"Response from Arduino: {response}")
    
    # Send a PUT command to change DEFTIME
    default_new_time = 360  # New time in seconds
    response = send_command_and_receive_data(f"DEFTIME {default_new_time}", arduino)
    print(f"Response from Arduino: {response}")
    
    # Send a PUT command to change BONUS
    bonus_new_time = 90  # New time in seconds
    response = send_command_and_receive_data(f"BONUS {bonus_new_time}", arduino)
    print(f"Response from Arduino: {response}")
    
    # Send a PAUSE command
    response = send_command_and_receive_data("PAUSE", arduino)
    print(f"Response from Arduino: {response}")
    
    arduino.close()
