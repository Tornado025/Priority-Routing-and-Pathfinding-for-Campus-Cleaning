import ctypes
import sys

from bindings import (
    MAX_NODES,
    MAX_WORKERS,
    PriorityQueue,
    Worker,
    graph,
    lib,
    room_names,
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QColor, QFont, QIntValidator, QPainter, QPen
from PySide6.QtWidgets import (
    QApplication,
    QFrame,
    QHBoxLayout,
    QHeaderView,
    QLabel,
    QLineEdit,
    QMainWindow,
    QPushButton,
    QScrollArea,
    QTableWidget,
    QTableWidgetItem,
    QTextEdit,
    QVBoxLayout,
    QWidget,
)

BLUE = "#294072"
BLUE_HOVER = "#121B34"
BG = "#F5F5F5"
CARD = "#FFFFFF"
TEXT = "#1F2937"
TEXT_LIGHT = "#6B7280"
BORDER = "#E5E7EB"
EMERGENCY_RED = "#DC2626"
EMERGENCY_HOVER = "#DDDDDD"


class GraphWidget(QWidget):
    def __init__(self):
        super().__init__()
        self.setMinimumSize(420, 380)
        self.nodes = {}
        self.edges = []

    def update_graph(self):
        self.nodes.clear()
        self.edges.clear()
        positions = {
            6: (80, 40),      # Ground
            0: (40, 140),     # Kotak Lab
            4: (40, 240),     # Admin Block
            5: (190, 50),     # CS Block
            1: (125, 140),    # CV Block
            11: (125, 325),   # Mech Block (moved down for -45° angle with Admin)
            3: (300, 60),     # EE Block (top of triangle)
            12: (210, 260),   # IEM Block (adjusted slightly)
            10: (210, 340),   # BT Quadrangle
            2: (220, 150),    # EC Block (bottom left of triangle)
            9: (295, 340),    # Canteen
            7: (380, 150),    # ET Block (bottom right of triangle)
            8: (380, 240),    # Chem Block
        }
        for i in range(13):
            name = bytes(room_names[i]).decode("utf-8").rstrip("\x00")
            if name:
                self.nodes[i] = {"name": name, "pos": positions.get(i, (200, 200))}
        for i in range(MAX_NODES):
            for j in range(i + 1, MAX_NODES):
                weight = graph[i][j]
                if weight > 0 and i in self.nodes and j in self.nodes:
                    self.edges.append((i, j, weight))
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)

        painter.setPen(QPen(QColor(BORDER), 2))
        for i, j, weight in self.edges:
            pos1 = self.nodes[i]["pos"]
            pos2 = self.nodes[j]["pos"]
            painter.drawLine(pos1[0], pos1[1], pos2[0], pos2[1])
            mid_x = (pos1[0] + pos2[0]) // 2
            mid_y = (pos1[1] + pos2[1]) // 2
            painter.setFont(QFont("Helvetica Neue", 9))
            painter.setPen(QColor(TEXT_LIGHT))
            painter.drawText(mid_x - 5, mid_y - 5, str(weight))
            painter.setPen(QPen(QColor(BORDER), 2))

        for node_id, node_data in self.nodes.items():
            pos = node_data["pos"]
            painter.setBrush(QColor(BLUE))
            painter.setPen(Qt.NoPen)
            painter.drawEllipse(pos[0] - 30, pos[1] - 30, 60, 60)

            painter.setPen(QColor("#FFFFFF"))
            painter.setFont(QFont("Helvetica Neue", 11, QFont.Bold))
            text = f"{node_id}"
            fm = painter.fontMetrics()
            tw = fm.horizontalAdvance(text)
            painter.drawText(pos[0] - tw // 2, pos[1] - 2, text)

            painter.setFont(QFont("Helvetica Neue", 9))
            name = node_data["name"][:10]
            fm2 = painter.fontMetrics()
            tw2 = fm2.horizontalAdvance(name)
            painter.drawText(pos[0] - tw2 // 2, pos[1] + 14, name)


class RequestCard(QFrame):
    def __init__(self, index, req, room_name):
        super().__init__()
        self.setObjectName("RequestCard")
        # Restored the border here
        self.setStyleSheet(
            f"""
            #RequestCard {{
                background-color: {CARD};
                border: 1px solid {BORDER};
                border-radius: 8px;
            }}
            """
        )
        layout = QVBoxLayout(self)
        layout.setContentsMargins(12, 10, 12, 10)
        layout.setSpacing(4)

        top = QHBoxLayout()
        idx_label = QLabel(f"[{index}]")
        idx_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px;"
        )
        top.addWidget(idx_label)

        if req.is_emergency:
            badge = QLabel("Emergency")
            badge.setStyleSheet(
                f"background-color: {EMERGENCY_RED}; color: white; padding: 2px 8px; border-radius: 4px; font-family: 'Helvetica Neue'; font-size: 9px; font-weight: bold;"
            )
        else:
            badge = QLabel("Routine")
            badge.setStyleSheet(
                f"background-color: {BLUE}; color: white; padding: 2px 8px; border-radius: 4px; font-family: 'Helvetica Neue'; font-size: 9px;"
            )
        top.addWidget(badge)
        top.addStretch()
        layout.addLayout(top)

        room_label = QLabel(f"Room {req.room_id}: {room_name}")
        room_label.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 12px; font-weight: bold;"
        )
        layout.addWidget(room_label)

        skill_names = []
        if req.skill_required & 1:
            skill_names.append("Janitor")
        if req.skill_required & 2:
            skill_names.append("Security")
        if req.skill_required & 4:
            skill_names.append("Manager")
        if req.skill_required & 8:
            skill_names.append("Admin")
        skill_label = QLabel(f"Skills: {'+'.join(skill_names)}")
        skill_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 11px;"
        )
        layout.addWidget(skill_label)

        stats = QLabel(
            f"Priority: {req.priority} → {req.effective_priority}  •  Waited: {req.wait_count} cycles"
        )
        stats.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px; font-weight: light;"
        )
        layout.addWidget(stats)

        cause = req.cause.decode("utf-8").rstrip("\x00")
        if cause:
            cause_label = QLabel(cause)
            cause_label.setStyleSheet(
                f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px; font-style: italic;"
            )
            cause_label.setWordWrap(True)
            layout.addWidget(cause_label)


class WorkerHashmapWidget(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(8)

        header = QLabel("Worker Hashmap")
        header.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 18px; font-weight: bold;"
        )
        layout.addWidget(header)

        self.table = QTableWidget()
        self.table.setColumnCount(6)
        self.table.setHorizontalHeaderLabels(["ID", "Name", "Room", "Skills", "Occupied", "Jobs"])
        self.table.setStyleSheet(
            f"""
            QTableWidget {{
                background-color: {CARD};
                border: none;
                gridline-color: {BORDER};
                font-family: 'Helvetica Neue';
                font-size: 13px;
            }}
            QTableWidget::item {{
                padding: 8px;
                color: {TEXT};
            }}
            QHeaderView::section {{
                background-color: {BG};
                color: {TEXT};
                font-family: 'Helvetica Neue';
                font-size: 12px;
                font-weight: bold;
                padding: 8px;
                border: none;
                border-bottom: 1px solid {BORDER};
            }}
        """
        )
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        self.table.verticalHeader().setVisible(False)
        self.table.setSelectionMode(QTableWidget.NoSelection)
        self.table.setEditTriggers(QTableWidget.NoEditTriggers)
        layout.addWidget(self.table)

    def update_workers(self):
        workers_ptr = (ctypes.POINTER(Worker) * MAX_WORKERS)()
        count = ctypes.c_int()
        lib.get_all_workers(workers_ptr, ctypes.byref(count))

        self.table.setRowCount(count.value)
        for i in range(count.value):
            if workers_ptr[i]:
                w = workers_ptr[i].contents
                self.table.setItem(i, 0, QTableWidgetItem(str(w.worker_id)))
                name = bytes(w.name).decode("utf-8").rstrip("\x00")
                self.table.setItem(i, 1, QTableWidgetItem(name))

                if w.current_room_id >= 0:
                    room_name = (
                        bytes(room_names[w.current_room_id])
                        .decode("utf-8")
                        .rstrip("\x00")
                    )
                    room_display = f"{w.current_room_id}: {room_name}"
                else:
                    room_display = "-"
                self.table.setItem(i, 2, QTableWidgetItem(room_display))

                skill_names = []
                if w.skill_type & 1:
                    skill_names.append("J")
                if w.skill_type & 2:
                    skill_names.append("S")
                if w.skill_type & 4:
                    skill_names.append("M")
                if w.skill_type & 8:
                    skill_names.append("A")
                self.table.setItem(i, 3, QTableWidgetItem("+".join(skill_names)))

                self.table.setItem(
                    i, 4, QTableWidgetItem("Yes" if w.is_occupied else "No")
                )
                self.table.setItem(i, 5, QTableWidgetItem(str(w.jobs_completed)))


class PriorityQueueWidget(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(8)

        header = QLabel("Priority Queue (Max Heap)")
        header.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 18px; font-weight: bold;"
        )
        layout.addWidget(header)

        self.info_label = QLabel()
        self.info_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 13px; font-weight: light;"
        )
        layout.addWidget(self.info_label)

        scroll = QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setStyleSheet("QScrollArea { border: none; background: transparent; }")

        self.container = QWidget()
        self.container.setStyleSheet("QWidget { background: transparent; }")
        self.cards_layout = QVBoxLayout(self.container)
        self.cards_layout.setContentsMargins(8, 8, 8, 8)
        self.cards_layout.setSpacing(8)
        self.cards_layout.addStretch()

        scroll.setWidget(self.container)
        layout.addWidget(scroll)

    def update_queue(self, pq):
        self.info_label.setText(f"Size: {pq.size}  •  Counter: {pq.counter}")

        while self.cards_layout.count() > 1:
            item = self.cards_layout.takeAt(0)
            if item.widget():
                item.widget().deleteLater()

        for i in range(pq.size):
            req = pq.data[i]
            room = bytes(room_names[req.room_id]).decode("utf-8").rstrip("\x00")
            card = RequestCard(i, req, room)
            self.cards_layout.insertWidget(i, card)


class LogWidget(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(8)

        header = QLabel("Operation Log")
        header.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 18px; font-weight: bold;"
        )
        layout.addWidget(header)

        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.log.setStyleSheet(
            f"""
            QTextEdit {{
                background-color: {CARD};
                color: {TEXT};
                border: 1px solid {BORDER};
                border-radius: 8px;
                font-family: 'Helvetica Neue';
                font-size: 13px;
                font-weight: light;
                padding: 12px;
            }}
        """
        )
        layout.addWidget(self.log)

    def append(self, msg):
        self.log.append(msg)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Campus Cleaning Scheduler")
        self.setGeometry(100, 100, 1400, 900)
        self.setObjectName("MainWindow")

        # FIX: Restrict background color to #MainWindow only
        # Added QLabel { background: transparent } to prevent labels from turning gray
        self.setStyleSheet(
            f"""
            #MainWindow {{ background-color: {BG}; }}
            QLabel {{ background-color: transparent; border: none; }}
            """
        )

        self.pq = PriorityQueue()
        lib.init_system()
        lib.initPQ(ctypes.byref(self.pq))

        central = QWidget()
        self.setCentralWidget(central)
        main_layout = QVBoxLayout(central)
        main_layout.setSpacing(16)
        main_layout.setContentsMargins(20, 20, 20, 20)

        state_container = QWidget()
        state_layout = QHBoxLayout(state_container)
        state_layout.setContentsMargins(0, 0, 0, 0)
        state_layout.setSpacing(16)

        # Graph Card
        graph_card = QFrame()
        graph_card.setObjectName("Container")
        graph_card.setStyleSheet(
            f"#Container {{ background-color: {CARD}; border: 1px solid {BORDER}; border-radius: 12px; }}"
        )
        graph_layout = QVBoxLayout(graph_card)
        graph_layout.setContentsMargins(16, 16, 16, 16)
        graph_layout.setSpacing(8)

        graph_header = QLabel("Campus Graph")
        graph_header.setFixedHeight(30)
        graph_header.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 18px; font-weight: bold;"
        )
        graph_layout.addWidget(graph_header)

        self.graph_widget = GraphWidget()
        graph_layout.addWidget(self.graph_widget)
        state_layout.addWidget(graph_card)

        # Right side with worker hashmap and queue
        right_side = QWidget()
        right_layout = QVBoxLayout(right_side)
        right_layout.setContentsMargins(0, 0, 0, 0)
        right_layout.setSpacing(16)

        # Worker Hashmap Card
        worker_card = QFrame()
        worker_card.setObjectName("Container")
        worker_card.setStyleSheet(
            f"#Container {{ background-color: {CARD}; border: 1px solid {BORDER}; border-radius: 12px; }}"
        )
        worker_layout = QVBoxLayout(worker_card)
        worker_layout.setContentsMargins(16, 16, 16, 16)
        self.worker_widget = WorkerHashmapWidget()
        worker_layout.addWidget(self.worker_widget)
        right_layout.addWidget(worker_card)

        # Queue Card
        queue_card = QFrame()
        queue_card.setObjectName("Container")
        queue_card.setStyleSheet(
            f"#Container {{ background-color: {CARD}; border: 1px solid {BORDER}; border-radius: 12px; }}"
        )
        queue_layout = QVBoxLayout(queue_card)
        queue_layout.setContentsMargins(16, 16, 16, 16)
        self.queue_widget = PriorityQueueWidget()
        queue_layout.addWidget(self.queue_widget)
        right_layout.addWidget(queue_card, 1)

        state_layout.addWidget(right_side)

        state_layout.setStretch(0, 1)
        state_layout.setStretch(1, 1)
        main_layout.addWidget(state_container, 65)

        controls = QWidget()
        controls_layout = QHBoxLayout(controls)
        controls_layout.setContentsMargins(0, 0, 0, 0)
        controls_layout.setSpacing(16)

        general = QWidget()
        general_layout = QVBoxLayout(general)
        general_layout.setContentsMargins(0, 0, 0, 0)
        general_layout.setSpacing(10)

        gen_header = QLabel("General Operations")
        gen_header.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 18px; font-weight: bold;"
        )
        general_layout.addWidget(gen_header)

        btn_init = QPushButton("Initialize Routine Jobs")
        btn_init.setMinimumHeight(44)
        btn_init.setCursor(Qt.PointingHandCursor)
        btn_init.setStyleSheet(self._button_style())
        btn_init.clicked.connect(self.init_routine)
        general_layout.addWidget(btn_init)

        btn_exec = QPushButton("Execute Next Job")
        btn_exec.setMinimumHeight(44)
        btn_exec.setCursor(Qt.PointingHandCursor)
        btn_exec.setStyleSheet(self._button_style())
        btn_exec.clicked.connect(self.execute_job)
        general_layout.addWidget(btn_exec)

        btn_update = QPushButton("Age / Update Priorities")
        btn_update.setMinimumHeight(44)
        btn_update.setCursor(Qt.PointingHandCursor)
        btn_update.setStyleSheet(self._button_style())
        btn_update.clicked.connect(self.update_priorities)
        general_layout.addWidget(btn_update)

        general_layout.addStretch()
        controls_layout.addWidget(general, 1)

        emergency = QWidget()
        emergency_layout = QVBoxLayout(emergency)
        emergency_layout.setContentsMargins(0, 0, 0, 0)
        emergency_layout.setSpacing(10)

        emg_header = QLabel("Add Emergency Job")
        emg_header.setStyleSheet(
            f"color: {TEXT}; font-family: 'Helvetica Neue'; font-size: 18px; font-weight: bold;"
        )
        emergency_layout.addWidget(emg_header)

        inputs_row = QHBoxLayout()
        inputs_row.setSpacing(12)

        room_container = QVBoxLayout()
        room_label = QLabel("Room (0-12)")
        room_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px;"
        )
        self.room_input = QLineEdit()
        self.room_input.setPlaceholderText("0")
        self.room_input.setMinimumHeight(36)
        self.room_input.setMaximumWidth(80)
        self.room_input.setValidator(QIntValidator(0, 12))
        self.room_input.setStyleSheet(self._input_style())
        room_container.addWidget(room_label)
        room_container.addWidget(self.room_input)
        inputs_row.addLayout(room_container)

        prio_container = QVBoxLayout()
        prio_label = QLabel("Priority (9-10)")
        prio_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px;"
        )
        self.prio_input = QLineEdit()
        self.prio_input.setPlaceholderText("9")
        self.prio_input.setMinimumHeight(36)
        self.prio_input.setMaximumWidth(90)
        self.prio_input.setValidator(QIntValidator(9, 10))
        self.prio_input.setStyleSheet(self._input_style())
        prio_container.addWidget(prio_label)
        prio_container.addWidget(self.prio_input)
        inputs_row.addLayout(prio_container)

        skill_container = QVBoxLayout()
        skill_label = QLabel("Skill (1-15)")
        skill_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px;"
        )
        self.skill_input = QLineEdit()
        self.skill_input.setPlaceholderText("1")
        self.skill_input.setMinimumHeight(36)
        self.skill_input.setMaximumWidth(80)
        self.skill_input.setValidator(QIntValidator(1, 15))
        self.skill_input.setStyleSheet(self._input_style())
        skill_container.addWidget(skill_label)
        skill_container.addWidget(self.skill_input)
        inputs_row.addLayout(skill_container)

        cause_container = QVBoxLayout()
        cause_label = QLabel("Cause")
        cause_label.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 12px;"
        )
        self.cause_input = QLineEdit()
        self.cause_input.setPlaceholderText("Description...")
        self.cause_input.setMinimumHeight(36)
        self.cause_input.setStyleSheet(self._input_style())
        cause_container.addWidget(cause_label)
        cause_container.addWidget(self.cause_input)
        inputs_row.addLayout(cause_container, 1)

        emergency_layout.addLayout(inputs_row)

        skill_help = QLabel(
            "Skills:\n"
            "  1 = Janitor    2 = Security    4 = Manager    8 = Admin\n"
            "  Add numbers for multiple skills (e.g., 9 = Janitor + Admin)"
        )
        skill_help.setStyleSheet(
            f"color: {TEXT_LIGHT}; font-family: 'Helvetica Neue'; font-size: 11px; padding: 8px 0px; line-height: 1.5;"
        )
        emergency_layout.addWidget(skill_help)

        btn_emg = QPushButton("Add Emergency")
        btn_emg.setMinimumHeight(40)
        btn_emg.setCursor(Qt.PointingHandCursor)
        btn_emg.setStyleSheet(self._button_style(emergency=True))
        btn_emg.clicked.connect(self.add_emergency)
        emergency_layout.addWidget(btn_emg)

        emergency_layout.addStretch()
        controls_layout.addWidget(emergency, 1)

        # Log Card
        log_card = QFrame()
        log_card.setObjectName("Container")
        log_card.setStyleSheet(
            f"#Container {{ background-color: {CARD}; border: 1px solid {BORDER}; border-radius: 12px; }}"
        )
        log_layout = QVBoxLayout(log_card)
        log_layout.setContentsMargins(16, 16, 16, 16)
        self.log_widget = LogWidget()
        log_layout.addWidget(self.log_widget)
        controls_layout.addWidget(log_card, 1)

        main_layout.addWidget(controls, 35)
        self.refresh_ui()

    def _button_style(self, emergency=False):
        bg = CARD if emergency else BLUE
        hover = EMERGENCY_HOVER if emergency else BLUE_HOVER
        border = f"1px solid {EMERGENCY_RED}" if emergency else "none"
        text_color = EMERGENCY_RED if emergency else "white"
        return f"""
            QPushButton {{
                background-color: {bg};
                color: {text_color};
                border: {border};
                border-radius: 8px;
                padding: 12px 20px;
                font-family: 'Helvetica Neue';
                font-size: 13px;
                font-weight: bold;
            }}
            QPushButton:hover {{ background-color: {hover}; }}
            QPushButton:pressed {{ background-color: {TEXT}; }}
        """

    def _input_style(self):
        return f"""
            QLineEdit {{
                background-color: {CARD};
                color: {TEXT};
                border: 1px solid {BORDER};
                border-radius: 6px;
                padding: 8px;
                font-family: 'Helvetica Neue';
                font-size: 13px;
            }}
            QLineEdit:focus {{ border: 1px solid {BLUE}; }}
        """

    def refresh_ui(self):
        self.graph_widget.update_graph()
        self.worker_widget.update_workers()
        self.queue_widget.update_queue(self.pq)

    def log_msg(self, msg):
        self.log_widget.append(msg)

    def init_routine(self):
        lib.init_routine_jobs(ctypes.byref(self.pq))
        self.log_msg("Initialized 13 routine jobs")
        self.refresh_ui()

    def add_emergency(self):
        try:
            room = int(self.room_input.text() or "0")
            room = max(0, min(12, room))
        except ValueError:
            room = 0
        try:
            priority = int(self.prio_input.text() or "9")
            priority = max(9, min(10, priority))
        except ValueError:
            priority = 9
        try:
            skill = int(self.skill_input.text() or "1")
            skill = max(1, min(15, skill))
        except ValueError:
            skill = 1
        cause = self.cause_input.text() or "Emergency"
        lib.add_emergency_job(
            ctypes.byref(self.pq), room, priority, skill, cause.encode("utf-8")
        )
        room_name = bytes(room_names[room]).decode("utf-8").rstrip("\x00")
        self.log_msg(f"Added emergency: Room {room} ({room_name})")
        self.room_input.clear()
        self.prio_input.clear()
        self.skill_input.clear()
        self.cause_input.clear()
        self.refresh_ui()

    def execute_job(self):
        lib.update_job_priorities(ctypes.byref(self.pq))
        req = lib.dequeue(ctypes.byref(self.pq))

        if req.room_id == -1:
            self.log_msg("Queue empty")
            return

        room_name = bytes(room_names[req.room_id]).decode("utf-8").rstrip("\x00")
        worker_id = lib.find_best_available_worker(req.room_id, req.skill_required)

        if worker_id == -1:
            self.log_msg("No workers with required skills, re-queued")
            lib.enqueue(
                ctypes.byref(self.pq),
                req.room_id,
                req.priority,
                req.is_emergency,
                req.skill_required,
                req.cause,
            )
        else:
            worker = lib.get_worker(worker_id).contents
            worker_name = bytes(worker.name).decode("utf-8").rstrip("\x00")
            self.log_msg(f"Worker {worker_name} → Room {req.room_id} ({room_name})")
            worker.current_room_id = req.room_id
            worker.jobs_completed += 1
            if not req.is_emergency:
                lib.enqueue(
                    ctypes.byref(self.pq), req.room_id, req.priority, 0, req.skill_required, req.cause
                )
        self.refresh_ui()

    def update_priorities(self):
        lib.update_job_priorities(ctypes.byref(self.pq))
        self.log_msg("Aged job priorities")
        self.refresh_ui()


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
