import tkinter as tk
from tkinter import filedialog, messagebox
import subprocess
import tempfile
import os
import sys

# Determina o nome do executável do compilador conforme o sistema operacional
if sys.platform.startswith('win'):
    compiler_executable = 'main_program.exe'  # Certifique-se de que o caminho está correto
else:
    compiler_executable = './main_program'    # Certifique-se de que o caminho está correto e o arquivo é executável

# Cria a janela principal
root = tk.Tk()
root.title('Compilador')

# Define a fonte monoespaçada para melhor legibilidade do código
font = ('Courier New', 10)

# Função para importar um programa de um arquivo
def import_program():
    filepath = filedialog.askopenfilename(
        title="Importar Programa",
        filetypes=[("Arquivos de Texto", "*.txt"), ("Todos os Arquivos", "*.*")]
    )
    if filepath:
        try:
            with open(filepath, 'r', encoding='utf-8') as file:
                code = file.read()
                code_editor.delete(1.0, tk.END)
                code_editor.insert(tk.END, code)
                update_line_numbers()
        except Exception as e:
            messagebox.showerror('Erro', f'Não foi possível abrir o arquivo:\n{e}')

# Função para salvar o programa em um arquivo
def save_program():
    filepath = filedialog.asksaveasfilename(
        defaultextension=".txt",
        filetypes=[("Arquivos de Texto", "*.txt"), ("Todos os Arquivos", "*.*")]
    )
    if filepath:
        try:
            with open(filepath, 'w', encoding='utf-8') as file:
                code = code_editor.get(1.0, tk.END)
                file.write(code)
        except Exception as e:
            messagebox.showerror('Erro', f'Não foi possível salvar o arquivo:\n{e}')

# Função para compilar o programa
def compile_program():
    code = code_editor.get(1.0, tk.END)
    # Cria um arquivo temporário para armazenar o código-fonte
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.txt', encoding='utf-8') as temp_file:
        temp_file.write(code)
        temp_filename = temp_file.name
    try:
        # Executa o compilador como um subprocesso
        result = subprocess.run(
            [compiler_executable, temp_filename],
            capture_output=True,
            text=True
        )
        # Exibe a saída na área de mensagens
        output_area.configure(state='normal')
        output_area.delete(1.0, tk.END)
        if result.returncode == 0:
            output_area.insert(tk.END, 'Compilação bem-sucedida.\n')
            if result.stdout:
                output_area.insert(tk.END, result.stdout)
            # Verifica se o arquivo .obj foi criado
            if os.path.exists('programa.obj'):
                output_area.insert(tk.END, 'Arquivo objeto "programa.obj" gerado com sucesso.\n')
            else:
                output_area.insert(tk.END, 'Aviso: O arquivo objeto não foi encontrado.\n')
        else:
            output_area.insert(tk.END, 'Erro de compilação:\n')
            if result.stderr:
                output_area.insert(tk.END, result.stderr)
            if result.stdout:
                output_area.insert(tk.END, result.stdout)
        output_area.configure(state='disabled')
    except Exception as e:
        messagebox.showerror('Erro', f'Erro ao executar o compilador:\n{e}')
    finally:
        # Remove o arquivo temporário
        os.unlink(temp_filename)

# Função para atualizar os números das linhas
def update_line_numbers(event=None):
    line_numbers = get_line_numbers()
    line_number_bar.configure(state='normal')
    line_number_bar.delete('1.0', tk.END)
    line_number_bar.insert('1.0', line_numbers)
    line_number_bar.configure(state='disabled')

def get_line_numbers():
    output = ''
    row, _ = code_editor.index("end").split('.')
    for i in range(1, int(row)):
        output += f'{i}\n'
    return output

# Sincroniza a rolagem do editor de código e da barra de números de linha
def on_scroll(*args):
    code_editor.yview(*args)
    line_number_bar.yview(*args)

def on_editor_scroll(*args):
    line_number_bar.yview_moveto(args[0])
    code_editor.yview_moveto(args[0])

# Cria o menu superior
menu_bar = tk.Menu(root)
file_menu = tk.Menu(menu_bar, tearoff=0)
file_menu.add_command(label="Novo", command=lambda: (code_editor.delete(1.0, tk.END), update_line_numbers()))
file_menu.add_command(label="Abrir", command=import_program)
file_menu.add_command(label="Salvar", command=save_program)
file_menu.add_separator()
file_menu.add_command(label="Sair", command=root.quit)
menu_bar.add_cascade(label="Arquivo", menu=file_menu)
root.config(menu=menu_bar)

# Cria o frame para os botões
frame_buttons = tk.Frame(root)
frame_buttons.pack(pady=5)

# Botão para importar programa
import_button = tk.Button(frame_buttons, text='Importar Programa', command=import_program)
import_button.pack(side=tk.LEFT, padx=5)

# Botão para salvar programa
save_button = tk.Button(frame_buttons, text='Salvar Programa', command=save_program)
save_button.pack(side=tk.LEFT, padx=5)

# Botão para compilar
compile_button = tk.Button(frame_buttons, text='Compilar', command=compile_program)
compile_button.pack(side=tk.LEFT, padx=5)

# Cria um frame para o editor de código e a barra de números de linha
editor_frame = tk.Frame(root)
editor_frame.pack(padx=10, pady=5, fill='both', expand=True)

# Barra de números de linha
line_number_bar = tk.Text(editor_frame, width=4, padx=3, takefocus=0, border=0, background='lightgrey', state='disabled', font=font)
line_number_bar.pack(side='left', fill='y')

# Cria o editor de código com barra de rolagem
code_editor_scrollbar = tk.Scrollbar(editor_frame)
code_editor_scrollbar.pack(side='right', fill='y')

code_editor = tk.Text(editor_frame, wrap='none', font=font, undo=True, yscrollcommand=code_editor_scrollbar.set)
code_editor.pack(side='right', fill='both', expand=True)

code_editor_scrollbar.config(command=on_scroll)

# Vincula eventos para atualizar os números de linha e sincronizar a rolagem
code_editor.bind('<KeyRelease>', update_line_numbers)
code_editor.bind('<MouseWheel>', update_line_numbers)
code_editor.bind('<Return>', update_line_numbers)
code_editor.bind('<BackSpace>', update_line_numbers)
code_editor.bind('<Delete>', update_line_numbers)

code_editor['yscrollcommand'] = on_editor_scroll
line_number_bar['yscrollcommand'] = on_editor_scroll

# Área de mensagens com barra de rolagem (somente leitura)
output_area = tk.Text(root, height=10, width=80, state='disabled', font=font)
output_area.pack(padx=10, pady=5, fill='both', expand=True)

# Inicia o loop principal da interface
update_line_numbers()
root.mainloop()
