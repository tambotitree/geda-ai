import sys
from PySide6.QtCore import Qt, QRectF
from PySide6.QtWidgets import (QApplication, QMainWindow, QWidget, QSplitter, 
                             QVBoxLayout, QHBoxLayout, QGraphicsView, QGraphicsScene, 
                             QGraphicsItem, QMenuBar, QTextEdit, QLineEdit, 
                             QLabel, QListWidget, QToolBox, QFormLayout)
from PySide6.QtGui import QAction, QPen, QColor, QBrush

class SchematicCanvas(QGraphicsView):
    """
    The main drawing window (Area 2). Supports infinite panning,
    mouse-wheel zooming, and structured layer rendering.
    """
    def __init__(self):
        super().__init__()
        self.scene = QGraphicsScene(self)
        self.scene.setSceneRect(-5000, -5000, 10000, 10000)
        self.setScene(self.scene)
        
        # Modern flat look for the schematic grid
        self.setBackgroundBrush(QBrush(QColor("#FFFFFF"))) 
        self.setRenderHint(self.renderHints().Antialiasing)
        self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)
        self.setResizeAnchor(QGraphicsView.AnchorUnderMouse)
        self.setDragMode(QGraphicsView.ScrollHandDrag) # Drag canvas to pan
        
        # Defined graphic layers for rendering
        self.layers = {
            "grid": self.scene.addGroup([]),
            "components": self.scene.addGroup([]),
            "nets": self.scene.addGroup([]),
            "ratlines": self.scene.addGroup([])
        }
        self.draw_grid()

    def draw_grid(self):
        """Draws a clean, non-obtrusive dot grid common in EDA tools."""
        pen = QPen(QColor("#E0E0E0"), 1)
        for x in range(-1000, 1000, 50):
            for y in range(-1000, 1000, 50):
                # Add tiny dots instead of harsh solid lines
                dot = self.scene.addEllipse(x, y, 1, 1, pen)
                self.layers["grid"].addToGroup(dot)

    def wheelEvent(self, event):
        """Implements smooth mouse-wheel zooming on the schematic canvas."""
        zoom_factor = 1.15
        if event.angleDelta().y() > 0:
            self.scale(zoom_factor, zoom_factor)
        else:
            self.scale(1.0 / zoom_factor, 1.0 / zoom_factor)


class MainWindow(QMainWindow):
    """
    Main Application Window replicating the exact layout structure of gschm.
    """
    def __init__(self):
        super().__init__()
        self.setWindowTitle("gEDA-py Schematic Editor")
        self.resize(1100, 800)
        
        # --- 1. Top Menu Bar ---
        self.setup_menu_bar()
        
        # --- Core Layout Splitters ---
        # Main horizontal split: [Main + Bottom Workspace] | [Right Control Panel]
        main_horizontal_splitter = QSplitter(Qt.Horizontal)
        
        # Left-side vertical split: [Schematic Canvas] / [Bottom Terminal Console]
        left_vertical_splitter = QSplitter(Qt.Vertical)
        
        # --- 2. Main Drawing Window (Canvas) ---
        self.canvas = SchematicCanvas()
        left_vertical_splitter.addWidget(self.canvas)
        
        # --- 4. Bottom Command Window ---
        bottom_container = QWidget()
        bottom_layout = QVBoxLayout(bottom_container)
        bottom_layout.setContentsMargins(5, 5, 5, 5)
        bottom_layout.setSpacing(2)
        
        self.console_output = QTextEdit()
        self.console_output.setReadOnly(True)
        self.console_output.setPlaceholderText("gEDA-py console logs and linter conditions will display here...")
        self.console_output.setStyleSheet("font-family: 'Menlo'; font-size: 11px; background-color: #F8F9FA; border: 1px solid #E0E0E0;")
        
        self.command_input = QLineEdit()
        self.command_input.setPlaceholderText("Type command line instructions here (e.g., netlist, compile, place)...")
        self.command_input.setStyleSheet("font-family: 'Menlo'; font-size: 11px; border: 1px solid #CBD5E1;")
        self.command_input.returnPressed.connect(self.execute_command)
        
        bottom_layout.addWidget(self.console_output)
        bottom_layout.addWidget(self.command_input)
        
        left_vertical_splitter.addWidget(bottom_container)
        left_vertical_splitter.setSizes([600, 150]) # Default height proportions
        
        # --- 3. Right Panel (Blender-style expandable/collapsible toolboxes) ---
        right_toolbox = QToolBox()
        right_toolbox.setFixedWidth(280)
        right_toolbox.setStyleSheet("QToolBox::tab { background: #E2E8F0; border-radius: 3px; font-weight: bold; }")
        
        # Toolbox Accordion Section A: Component Selection Library
        self.component_list = QListWidget()
        self.component_list.addItems([
            "resistor-1.sym", "resistor-2.sym", "resistor-variable-1.sym",
            "npn-1.sym", "pnp-1.sym", "capacitor-1.sym", "diode-1.19n914.sym",
            "opamp-quad-1.sym", "battery-1.sym", "gnd-1.sym"
        ])
        self.component_list.setStyleSheet("border: none;")
        right_toolbox.addItem(self.component_list, "🔌 Component Library")
        
        # Toolbox Accordion Section B: Attributes Editor
        attributes_widget = QWidget()
        attr_layout = QFormLayout(attributes_widget)
        attr_layout.addRow(QLabel("Refdes:"), QLineEdit("R1"))
        attr_layout.addRow(QLabel("Value:"), QLineEdit("10k"))
        attr_layout.addRow(QLabel("Footprint:"), QLineEdit("0805"))
        attr_layout.addRow(QLabel("Device:"), QLineEdit("resistor"))
        right_toolbox.addItem(attributes_widget, "📝 Attributes & Properties")
        
        # Assemble the top-level frames
        main_horizontal_splitter.addWidget(left_vertical_splitter)
        main_horizontal_splitter.addWidget(right_toolbox)
        main_horizontal_splitter.setSizes([820, 280]) # Default width proportions
        
        self.setCentralWidget(main_horizontal_splitter)
        self.console_output.append("System Initialization complete. gEDA-py backend standing by.")

    def setup_menu_bar(self):
        """Constructs flat, native macOS file dropdown trees."""
        menu_bar = self.menuBar()
        
        file_menu = menu_bar.addMenu("File")
        new_action = QAction("New Schematic", self)
        open_action = QAction("Open File (.sch)...", self)
        save_action = QAction("Save System", self)
        file_menu.addAction(new_action)
        file_menu.addAction(open_action)
        file_menu.addSeparator()
        file_menu.addAction(save_action)
        
        edit_menu = menu_bar.addMenu("Edit")
        object_menu = menu_bar.addMenu("Add Object")
        tools_menu = menu_bar.addMenu("Tools")
        
    def execute_command(self):
        """Handles basic CLI interaction from the bottom console prompt."""
        command = self.command_input.text().strip()
        if command:
            self.console_output.append(f"<b>&gt; {command}</b>")
            # This is where your FastAPI server router logic hooks in!
            if command == "clear":
                self.console_output.clear()
            else:
                self.console_output.append(f"Command '{command}' sent to gEDA-AI parsing processor...")
            self.command_input.clear()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
