import serial
import time
import tkinter as tk
from tkinter import StringVar, messagebox
from threading import Thread

def connect_to_com_port(port):
    """Connects to the given COM port and attempts communication."""
    try:
        ser = serial.Serial(port, baudrate=115200, timeout=1)
        time.sleep(2)  # Wait for Arduino to initialize
        ser.write(b"LINK-LOGON\n")
        response = ser.readline().decode('utf-8').strip()
        if response == "LOGON CONFIRM":
            print(f"Connected to Arduino on {port}")
            return ser
        if response !="LOGON CONFIRM":
            print(f"Unexpected response: {response}")
            return ser
        else :
            print(f"No response on {port}")
        ser.close()
    except Exception as e:
        print(f"Error connecting to {port}: {e}")
    return None


class SerialApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Arduino Monitor")
        self.root.geometry("800x400")
        self.root.configure(bg="black")

        self.time_left_var = StringVar(value="XX:XX:XX")
        self.time_right_var = StringVar(value="XX:XX:XX")
        self.move_count = 0
        self.move_var = StringVar(value=f"Moves: {self.move_count}")

        self.create_layout()

        self.serial_connection = None
        self.serial_thread = None
        self.connected = False

        self.try_connect()

    def try_connect(self):
#        for port in ["COM3", "COM4", "COM5"]:  # Expand with potential ports
#            self.serial_connection = connect_to_com_port(port)
#            if self.serial_connection:
#                self.connected = True
#                break
        for i in range (1,257):
            port=("COM" + str(i))
            self.serial_connection = connect_to_com_port(port)
            i+=1
            if self.serial_connection:
                self.connected = True
                break
        if not self.connected:
            self.show_error("Failed to connect to Arduino.")
        else:
            self.serial_thread = Thread(target=self.read_serial, daemon=True)
            self.serial_thread.start()

    def create_layout(self):
        tk.Label(
            self.root, textvariable=self.time_left_var, font=("Helvetica", 16), fg="white", bg="black"
        ).grid(row=0, column=0, padx=10, pady=10, sticky="w")
        tk.Label(
            self.root, textvariable=self.move_var, font=("Helvetica", 20), fg="white", bg="black"
        ).grid(row=0, column=1, padx=10, pady=10)
        tk.Label(
            self.root, textvariable=self.time_right_var, font=("Helvetica", 16), fg="white", bg="black"
        ).grid(row=0, column=2, padx=10, pady=10, sticky="e")

    def read_serial(self):
        """Reads serial data in a separate thread."""
        while self.connected:
            try:
                if self.serial_connection.in_waiting > 0:
                    raw_data = self.serial_connection.readline().decode('utf-8').strip()
                    self.process_data(raw_data)
                time.sleep(0.1)
            except Exception as e:
                print(f"Error reading serial: {e}")
                self.connected = False
                break

    def process_data(self, data):
        try:
            parts = data.split(",")
            if len(parts) == 3:
                self.time_left_var.set(self.format_seconds_to_hhmmss(parts[0]))
                self.time_right_var.set(self.format_seconds_to_hhmmss(parts[1]))
                self.move_var.set(f"Moves: {int(parts[2])}")
            else:
                print(f"Invalid data: {data}")
        except Exception as e:
            print(f"Error processing data: {e}")

    @staticmethod
    def format_seconds_to_hhmmss(seconds):
        try:
            seconds = int(seconds)
            h, m, s = seconds // 3600, (seconds % 3600) // 60, seconds % 60
            return f"{h:02}:{m:02}:{s:02}"
        except ValueError:
            return "XX:XX:XX"

    def show_error(self, message):
        messagebox.showerror("Error", message)


if __name__ == "__main__":
    root = tk.Tk()
    app = SerialApp(root)
    root.mainloop()
