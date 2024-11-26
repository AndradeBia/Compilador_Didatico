import subprocess
import threading
import json
import os
from tkinter import Tk, Label, Text, END, simpledialog, Scrollbar, RIGHT, Y, BOTH, LEFT, VERTICAL, Button, Radiobutton, StringVar

class InterpreterGUI:
    def __init__(self, master):
        self.master = master
        master.title("Interpreter GUI")

        # Mode selection
        self.mode = StringVar(value="all")  # default to "all at once"
        self.step_radio = Radiobutton(master, text="passo a passo", variable=self.mode, value="step")
        self.step_radio.pack()
        self.all_radio = Radiobutton(master, text="tudo de uma vez", variable=self.mode, value="all")
        self.all_radio.pack()

        # Execute button
        self.execute_button = Button(master, text="Executar", command=self.start_execution)
        self.execute_button.pack()

        # Restart button
        self.restart_button = Button(master, text="Reiniciar", command=self.restart_execution)
        self.restart_button.pack()

        # Instruction Vector Display
        self.instruction_label = Label(master, text="Instructions:")
        self.instruction_label.pack()
        self.instruction_text = Text(master, height=15, width=100)
        self.instruction_text.pack()

        # Memory Vector Display
        self.memory_label = Label(master, text="Memory:")
        self.memory_label.pack()
        self.memory_text = Text(master, height=15, width=100)
        self.memory_text.pack()

        # Output Display
        self.output_label = Label(master, text="Output:")
        self.output_label.pack()
        self.output_text = Text(master, height=10, width=100)
        self.output_text.pack()

        # Initialize process to None
        self.process = None

        # Load and display initial instructions
        self.load_instructions()

        # Handle closing of the GUI
        self.master.protocol("WM_DELETE_WINDOW", self.on_closing)

    def load_instructions(self):
        # Read the instructions from 'programa.obj'
        if os.path.exists('programa.obj'):
            with open('programa.obj', 'r') as file:
                lines = file.readlines()

            # Process and display instructions
            self.instruction_text.delete('1.0', END)
            self.instructions = []
            for idx, line in enumerate(lines):
                if len(line.strip()) >= 20:
                    # Extract instruction components
                    rotulo = line[0:4].strip()
                    instrucao = line[4:12].strip()
                    arg1 = line[12:16].strip()
                    arg2 = line[16:20].strip()
                    # Store instruction
                    inst = {
                        'rotulo': rotulo,
                        'instrucao': instrucao,
                        'arg1': arg1,
                        'arg2': arg2
                    }
                    self.instructions.append(inst)
                    # Display instruction
                    display_line = f"{idx}: {instrucao} {arg1} {arg2}\n"
                    self.instruction_text.insert(END, display_line)
        else:
            self.instruction_text.insert(END, "programa.obj not found.")
            self.instructions = []

        # Initialize memory display
        self.memory_text.delete('1.0', END)
        self.memory = [0] * 300  # Assuming memory size of 300
        memory_display = ", ".join(map(str, self.memory))
        self.memory_text.insert(END, memory_display)

    def start_execution(self):
        if self.process is None:
            # Start the C++ process
            mode = self.mode.get()
            self.process = subprocess.Popen(
                ['./main.exe', mode],  # Replace with your executable's path
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=1,
                universal_newlines=True
            )

            # Start the output reading thread
            self.output_thread = threading.Thread(target=self.read_output)
            self.output_thread.daemon = True
            self.output_thread.start()

            # If in step mode, send "NEXT" command
            if mode == "step":
                self.process.stdin.write("NEXT\n")
                self.process.stdin.flush()
        else:
            # Process is already running
            if self.mode.get() == "step":
                self.process.stdin.write("NEXT\n")
                self.process.stdin.flush()
            else:
                # In "all at once" mode, do nothing
                pass

    def read_output(self):
        while True:
            line = self.process.stdout.readline()
            if line:
                self.process_line(line.strip())
            else:
                break

    def process_line(self, line):
        try:
            data = json.loads(line)
            if data.get("event") == "INPUT_REQUEST":
                self.master.after(0, self.handle_input_request)
            elif data.get("event") == "OUTPUT":
                self.master.after(0, self.update_output, data["value"])
            elif data.get("event") == "STATE_UPDATE":
                self.master.after(0, self.update_state, data)
            elif data.get("event") == "EXECUTION_FINISHED":
                self.master.after(0, self.execution_finished)
        except json.JSONDecodeError:
            # Handle non-JSON lines if necessary
            pass

    def handle_input_request(self):
        user_input = simpledialog.askinteger("Input Required", "Enter a number:")
        if user_input is not None:
            self.process.stdin.write(f"{user_input}\n")
            self.process.stdin.flush()
        else:
            # If user cancels, send a default value or handle accordingly
            self.process.stdin.write("0\n")
            self.process.stdin.flush()

    def update_state(self, data):
        # Update instruction vector
        self.instruction_text.delete('1.0', END)
        instructions = data.get("instrucoes", [])
        instruction_pointer = data.get("instruction_pointer", 0)

        # Highlight current instruction
        for idx, inst in enumerate(instructions):
            line = f"{idx}: {inst['instrucao']} {inst['arg1']} {inst['arg2']}\n"
            if idx == instruction_pointer:
                self.instruction_text.insert(END, line, 'highlight')
            else:
                self.instruction_text.insert(END, line)
        self.instruction_text.tag_configure('highlight', background='yellow')

        # Update memory vector
        self.memory_text.delete('1.0', END)
        memoria_dados = data.get("memoria_dados", [])
        memory_display = ", ".join(map(str, memoria_dados))
        self.memory_text.insert(END, memory_display)

    def update_output(self, value):
        self.output_text.insert(END, f"Output: {value}\n")
        self.output_text.see(END)

    def execution_finished(self):
        # Reset the process to None when execution is finished
        if self.process:
            self.process.terminate()
            self.process = None
        self.output_text.insert(END, "Execution finished.\n")
        self.output_text.see(END)

    def restart_execution(self):
        # Terminate the current process if running
        if self.process:
            self.process.terminate()
            self.process = None

        # Clear output display
        self.output_text.delete('1.0', END)
        self.output_text.insert(END, "Execution restarted.\n")

        # Reload instructions and reset memory
        self.load_instructions()

    def on_closing(self):
        # Terminate the C++ process if it's running
        if self.process:
            self.process.terminate()
            self.process = None
        self.master.destroy()

if __name__ == "__main__":
    root = Tk()
    gui = InterpreterGUI(root)
    root.mainloop()
