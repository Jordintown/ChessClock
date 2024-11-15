import serial
import time
import tkinter as tk
from tkinter import StringVar, messagebox
from threading import Thread


# Function to connect to the selected COM port and communicate
def connect_to_com_port(port):
    try:
        print(f"Trying port {port}...")
        ser = serial.Serial(port, baudrate=9600, timeout=1)
        time.sleep(2)  # Allow time for Arduino to reset
        ser.write(b"LINK-LOGON\n")
        response = ser.readline().decode('utf-8').strip()
        if response == "LOGON-CONFIRM":
            print(f"Connected to Arduino on {port}")
            return ser
        else:
            print(f"Unexpected response: {response}")
        ser.close()
    except Exception as e:
        print(f"Error connecting to {port}: {e}")
        return None


# GUI and background thread
class SerialApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Arduino Monitor")

        # Set a fixed window size
        self.root.geometry("800x400")

        # Set a dark background
        self.root.configure(bg="black")

        # Initialize variables with default "XX:XX:XX" values
        self.time_left_var = StringVar(value="XX:XX:XX")
        self.time_right_var = StringVar(value="XX:XX:XX")
        self.move_count = 0
        self.move_var = StringVar(value=f"Moves: {self.move_count}")

        # Layout
        self.create_layout()

        # Start periodic update check using after method
        self.update_gui()

        # Bind resize event to adjust font size dynamically
        self.root.bind("<Configure>", self.on_resize)

        # Store initial window size for resizing calculations
        self.last_width = self.root.winfo_width()
        self.last_height = self.root.winfo_height()

        # Try to connect to the COM port directly (COM3)
        self.serial_connection = connect_to_com_port("COM3")
        if not self.serial_connection:
            self.show_error("Failed to connect to COM3.")
            return

        # Start background thread to listen to serial data
        self.serial_thread = Thread(target=self.read_serial, daemon=True)
        self.serial_thread.start()

    def create_layout(self):
        # Left corner time
        self.time_left_label = tk.Label(
            self.root, textvariable=self.time_left_var, font=("Helvetica", 16), fg="white", bg="black"
        )
        self.time_left_label.grid(row=0, column=0, sticky="w", padx=10, pady=10)

        # Center move counter
        self.move_label = tk.Label(
            self.root, textvariable=self.move_var, font=("Helvetica", 20), fg="white", bg="black"
        )
        self.move_label.grid(row=0, column=1, padx=20, pady=10)

        # Right corner time
        self.time_right_label = tk.Label(
            self.root, textvariable=self.time_right_var, font=("Helvetica", 16), fg="white", bg="black"
        )
        self.time_right_label.grid(row=0, column=2, sticky="e", padx=10, pady=10)

        # Center the move counter vertically
        self.root.grid_rowconfigure(0, weight=1)
        self.root.grid_columnconfigure(1, weight=1)  # Centering move label

    def on_resize(self, event):
        # Only update the font size if the window size has stabilized
        width = event.width
        height = event.height

        # Check if the window size has changed significantly
        if abs(width - self.last_width) > 10 or abs(height - self.last_height) > 10:
            self.last_width = width
            self.last_height = height
            self.root.after(100, self.resize_font)

    def resize_font(self):
        """Adjust font size based on window size."""
        width = self.root.winfo_width()
        height = self.root.winfo_height()

        # Calculate font size as a percentage of the window size
        font_size = min(width, height) // 15  # Adjust font size calculation to fit better

        # Set the font size for all labels
        self.time_left_label.config(font=("Helvetica", font_size))
        self.time_right_label.config(font=("Helvetica", font_size))
        self.move_label.config(font=("Helvetica", font_size))

    def read_serial(self):
        if not self.serial_connection:
            return

        print("Connection established! Listening for data...")
        try:
            while True:
                if self.serial_connection.in_waiting > 0:
                    raw_data = self.serial_connection.readline().decode("utf-8").strip()
                    print("Data received:", raw_data)
                    self.process_data(raw_data)
                time.sleep(0.1)
        except Exception as e:
            self.show_error(f"Serial reading error: {e}")
        finally:
            self.serial_connection.close()
            print("Serial connection closed.")

    def process_data(self, data):
        try:
            # Split the data into parts using a comma
            parts = data.split(",")
            print(f"Processed parts: {parts}")
            if len(parts) == 3:
                # Update left and right times
                left_time = self.format_seconds_to_hhmmss(parts[0])
                right_time = self.format_seconds_to_hhmmss(parts[1])

                self.time_left_var.set(left_time)
                self.time_right_var.set(right_time)

                # Update move counter
                if parts[2].isdigit():
                    self.move_count = int(parts[2])
                    self.move_var.set(f"Moves: {self.move_count}")
                else:
                    print(f"Invalid move counter: {parts[2]}")
            else:
                print(f"Unexpected data format: {data}")
        except Exception as e:
            self.show_error(f"Error processing data: {e}")

    @staticmethod
    def format_seconds_to_hhmmss(seconds):
        """Convert seconds into hh:mm:ss format."""
        try:
            seconds = int(seconds)
            hours = seconds // 3600
            minutes = (seconds % 3600) // 60
            seconds = seconds % 60
            return f"{hours:02}:{minutes:02}:{seconds:02}"
        except ValueError:
            print(f"Invalid seconds value: {seconds}")
            return "XX:XX:XX"

    def update_gui(self):
        """Periodically update the GUI."""
        self.root.after(100, self.update_gui)

    def show_error(self, message):
        """Show an error dialog with the given message."""
        messagebox.showerror("Error", message)


# Main
if __name__ == "__main__":
    root = tk.Tk()
    app = SerialApp(root)
    root.mainloop()
