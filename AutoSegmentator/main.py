import os
import sys
import subprocess
import shutil
from PySide6.QtWidgets import QTextEdit
from PySide6.QtGui import QTextCursor
from PySide6.QtCore import (
    QObject,
    QThread,
    Signal
)
from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QApplication,
    QWidget,
    QLabel,
    QPushButton,
    QFileDialog,
    QVBoxLayout,
    QHBoxLayout,
    QCheckBox,
    QScrollArea,
    QLineEdit,
    QMessageBox,
    QProgressBar,
    QTreeWidget,
    QTreeWidgetItem,
    QProgressBar
)
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
ROOT_DIR = os.path.abspath(os.path.join(BASE_DIR, ".."))

# Portable Python
PYTHON_EXE = os.path.join(
    ROOT_DIR,
    "portable_python",
    "Scripts",
    "python.exe"
)

# Portable TotalSegmentator executable
TOTALSEG_EXE = os.path.join(
    ROOT_DIR,
    "portable_python",
    "Scripts",
    "TotalSegmentator.exe"
)

# Local weights
TOTALSEG_HOME = os.path.join(
    ROOT_DIR,
    ".totalsegmentator"
)

os.environ["TOTALSEG_WEIGHTS_PATH"] = os.path.join(
    ROOT_DIR,
    ".totalsegmentator"
)

os.environ["TOTALSEG_HOME"] = TOTALSEG_HOME

from PySide6.QtWidgets import (
    QApplication,
    QWidget,
    QLabel,
    QPushButton,
    QFileDialog,
    QVBoxLayout,
    QHBoxLayout,
    QCheckBox,
    QScrollArea,
    QLineEdit,
    QMessageBox
)



# ----------------------------------------------------
# Available structures
# ----------------------------------------------------REGIONS = {
REGIONS = {

    "Brain": {

        "Brain": {
            "task": "total_v3",
            "roi": "brain"
        },
        
        "Brain Stem": {
            "task": "brain_structures",
            "roi": "brainstem"
        },

        "Frontal Lobe": {
            "task": "brain_structures",
            "roi": "frontal_lobe"
        },
        
        "Parietal Lobe": {
            "task": "brain_structures",
            "roi": "parietal_lobe"
        },
        "Occipital Lobe": {
            "task": "brain_structures",
            "roi": "occipital_lobe"
        },
        "Temporal Lobe": {
            "task": "brain_structures",
            "roi": "temporal_lobe"
        },


        "Thalamus": {
            "task": "brain_structures",
            "roi": "thalamus"
        },

        "Eyeball Left": {
            "task": "oculomotor_muscles",
            "roi": "eyeball_left"
        },

        "Eyeball Right": {
            "task": "oculomotor_muscles",
            "roi": "eyeball_right"
        },

        "Eye Lens Left": {
            "task": "head_glands_cavities",
            "roi": "eye_lens_left"
        },

        "Eye Lens Right": {
            "task": "head_glands_cavities",
            "roi": "eye_lens_right"
        },

        "Optic Nerve Left": {
            "task": "oculomotor_muscles",
            "roi": "optic_nerve_left"
        },

        "Optic Nerve Right": {
            "task": "oculomotor_muscles",
            "roi": "optic_nerve_right"
        },

    },


    "Breast": {

        "Breast": {
            "task": "breasts",
            "roi": "breast"
        },

        "Spinal Cord": {
            "task": "total_v3",
            "roi": "spinal_cord"
        },

        "Left Lung": {
            "task": "total_v3",
            "roi": [
                "lung_upper_lobe_left",
                "lung_lower_lobe_left"
            ]
        },

        "Right Lung": {
            "task": "total_v3",
            "roi": [
                "lung_upper_lobe_right",
                "lung_middle_lobe_right",
                "lung_lower_lobe_right"
            ]
        },
        

        "Thyroid": {
            "task": "total_v3",
            "roi": "thyroid_gland"
        },

        "Esophagus": {
            "task": "total_v3",
            "roi": "esophagus"
        },

        "Heart": {
            "task": "total_v3",
            "roi": "heart"
        },

        "Liver": {
            "task": "total_v3",
            "roi": "liver"
        },

        "Trachea": {
            "task": "total_v3",
            "roi": "trachea"
        },
        "Spleen": {
                "task": "total",
                "roi": "spleen"
            }
    },


    "Head & Neck": {

        "Brain": {
            "task": "total_v3",
            "roi": "brain"
        },
        
        "Brain Stem": {
            "task": "brain_structures",
            "roi": "brainstem"
        },

        "Frontal Lobe": {
            "task": "brain_structures",
            "roi": "frontal_lobe"
        },
        
        "Parietal Lobe": {
            "task": "brain_structures",
            "roi": "parietal_lobe"
        },
        "Occipital Lobe": {
            "task": "brain_structures",
            "roi": "occipital_lobe"
        },
        "Temporal Lobe": {
            "task": "brain_structures",
            "roi": "temporal_lobe"
        },


        "Thalamus": {
            "task": "brain_structures",
            "roi": "thalamus"
        },
        "Eyeball Left": {
            "task": "oculomotor_muscles",
            "roi": "eyeball_left"
        },

        "Eyeball Right": {
            "task": "oculomotor_muscles",
            "roi": "eyeball_right"
        },

        "Eye Lens Left": {
            "task": "head_glands_cavities",
            "roi": "eye_lens_left"
        },

        "Eye Lens Right": {
            "task": "head_glands_cavities",
            "roi": "eye_lens_right"
        },

        "Optic Nerve Left": {
            "task": "oculomotor_muscles",
            "roi": "optic_nerve_left"
        },

        "Optic Nerve Right": {
            "task": "oculomotor_muscles",
            "roi": "optic_nerve_right"
        },

        "Parotid Left": {
            "task": "head_glands_cavities",
            "roi": "parotid_gland_left"
        },

        "Parotid Right": {
            "task": "head_glands_cavities",
            "roi": "parotid_gland_right"
        },

        "Spinal Cord": {
            "task": "total_v3",
            "roi": "spinal_cord"
        },

        "Left Lung Upper": {
            "task": "total_v3",
            "roi": "lung_upper_lobe_left"
        },
        
        "Left Lung": {
            "task": "total_v3",
            "roi": [
                "lung_upper_lobe_left",
                "lung_lower_lobe_left"
            ]
        },

        "Right Lung": {
            "task": "total_v3",
            "roi": [
                "lung_upper_lobe_right",
                "lung_middle_lobe_right",
                "lung_lower_lobe_right"
            ]
        },


        "Thyroid": {
            "task": "total_v3",
            "roi": "thyroid_gland"
        },

        "Esophagus": {
            "task": "total_v3",
            "roi": "esophagus"
        },

        "Trachea": {
            "task": "total_v3",
            "roi": "trachea"
        }
    },


    "Pelvis": {

        "Urinary Bladder": {
            "task": "total_v3",
            "roi": "urinary_bladder"
        },

        "Colon": {
            "task": "total_v3",
            "roi": "colon"
        },

        "Small Bowel": {
            "task": "total_v3",
            "roi": "small_bowel"
        },

        "Left Kidney": {
            "task": "total_v3",
            "roi": "kidney_left"
        },

        "Right Kidney": {
            "task": "total_v3",
            "roi": "kidney_right"
        },

        "Left Femur": {
            "task": "total_v3",
            "roi": "femur_left"
        },

        "Right Femur": {
            "task": "total_v3",
            "roi": "femur_right"
        },
        "Prostate": {
            "task": "total_v3",
            "roi": "prostate"
        },
        "Pancreas": {
            "task": "total_v3",
            "roi": "pancreas"
        },
        "Stomach": {
            "task": "total_v3",
            "roi": "stomach"
        }
    },


    "Thorax": {

        "Spinal Cord": {
            "task": "total_v3",
            "roi": "spinal_cord"
        },

            "Left Lung": {
                "task": "total_v3",
                "roi": [
                    "lung_upper_lobe_left",
                    "lung_lower_lobe_left"
                ]
            },

            "Right Lung": {
                "task": "total_v3",
                "roi": [
                    "lung_upper_lobe_right",
                    "lung_middle_lobe_right",
                    "lung_lower_lobe_right"
                ]
            },
        
        "Right Kidney": {
            "task": "total_v3",
            "roi": "kidney_right"
        },
        "Left Kidney": {
            "task": "total_v3",
            "roi": "kidney_left"
        },
        "Esophagus": {
            "task": "total_v3",
            "roi": "esophagus"
        },

        "Heart": {
            "task": "total_v3",
            "roi": "heart"
        },

        "Liver": {
            "task": "total_v3",
            "roi": "liver"
        },
        
        "Vertebrae": {
            "task": "vertebrae_pp",
            "roi": ["vertebrae_C1",
        "vertebrae_C2",
        "vertebrae_C3",
        "vertebrae_C4",
        "vertebrae_C5",
        "vertebrae_C6",
        "vertebrae_C7",
        "vertebrae_T1",
        "vertebrae_T2",
        "vertebrae_T3",
        "vertebrae_T4",
        "vertebrae_T5",
        "vertebrae_T6",
        "vertebrae_T7",
        "vertebrae_T8",
        "vertebrae_T9",
        "vertebrae_T10",
        "vertebrae_T11",
        "vertebrae_T12",
        "vertebrae_L1",
        "vertebrae_L2",
        "vertebrae_L3",
        "vertebrae_L4",
        "vertebrae_L5"
        ]
        },
        "Sacrum": {
            "task": "total_v3",
            "roi": [
                "sacrum"
            ]
        },       
        "Stomach": {
            "task": "total_v3",
            "roi": "stomach"
        },
        "Prostate": {
            "task": "total_v3",
            "roi": "prostate"
        },
        "Pancreas": {
            "task": "total_v3",
            "roi": "pancreas"
        },

        "Thyroid": {
            "task": "total_v3",
            "roi": "thyroid_gland"
        },
        
        "Trachea": {
            "task": "total_v3",
            "roi": "trachea"
        },
            "Spleen": {
                "task": "total",
                "roi": "spleen"
            }
    }
}


class Worker(QObject):

    finished = Signal()
    error = Signal(str)
    status = Signal(str)
    log = Signal(str)
    progress = Signal(int)

    def __init__(self, ct_folder, output_folder, tasks, use_gpu):
        super().__init__()
        
        self.use_gpu = use_gpu
        self.ct_folder = ct_folder
        self.output_folder = output_folder
        self.tasks = tasks


    def run(self):
    
        #
        # Delete previous masks
        #
        try:
                
            for file in os.listdir(self.output_folder):

                if file.endswith(".nii.gz"):

                    os.remove(
                        os.path.join(self.output_folder, file)
                    )

            #
            # Run every task
            #
            total_tasks = len(self.tasks)

            current_task = 0
            
            requested_masks = set()

            for task, rois in self.tasks.items():

                flat_rois = []
                    
                for roi in rois:

                    if isinstance(roi, list):
                        flat_rois.extend(roi)
                    else:
                        flat_rois.append(roi)

    
                for roi in flat_rois:
                    requested_masks.add(f"{roi}.nii.gz")

                    
                self.status.emit(f"Running {task}...")
                current_task += 1
                self.progress.emit(
                    int((current_task-1)/total_tasks*100)
                )

                if task in ("total", "total_v3"):

                    args = [

                        "-i", self.ct_folder,

                        "-o", self.output_folder,
                        "--task", "total",

                        "--roi_subset"

                    ]

                    args.extend(flat_rois)

                else:

                    args = [

                        "-i", self.ct_folder,

                        "-o", self.output_folder,

                        "--task", task

                    ]

                device = "gpu" if self.use_gpu else "cpu"

                args.extend([

                    "--robust_crop",

                    "--higher_order_resampling",

                    "--resampling_order", "3",

                    "--device", device

                ])

                command = [TOTALSEG_EXE] + args

                self.log.emit(" ".join(command))

                process = subprocess.Popen(
                    command,
                    stdout=subprocess.PIPE,
                    stderr=subprocess.STDOUT,
                    text=True,
                    encoding="utf-8",
                    errors="replace"
                )

                for line in process.stdout:
                    self.log.emit(line.rstrip())

                process.wait()

                if process.returncode != 0:
                    self.error.emit(
                        f"TotalSegmentator exited with code {process.returncode}"
                    )
                    return


            #
            # Remove masks that were not requested
            #

            self.status.emit("Cleaning output...")

            for filename in os.listdir(self.output_folder):

                if not filename.endswith(".nii.gz"):
                    continue

                if filename not in requested_masks:

                    try:

                        os.remove(
                            os.path.join(self.output_folder, filename)
                        )

                        self.log.emit(f"Deleted {filename}")

                    except Exception as ex:

                        self.log.emit(
                            f"Could not delete {filename}: {ex}"
                        )

            #
            # Convert
            #
            
            self.status.emit("Creating RTSTRUCT...")

            process = subprocess.Popen(
                [
                    PYTHON_EXE,
                    os.path.join(BASE_DIR, "convert.py"),
                    self.ct_folder,
                    self.output_folder
                ],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding="utf-8",
                errors="replace"
            )

            for line in process.stdout:
                self.log.emit(line.rstrip())

            process.wait()

            if process.returncode != 0:
                self.error.emit("convert.py failed.")
                return
                
            self.progress.emit(100)

            self.finished.emit()
            self.log.emit("")
            self.log.emit("======================")
            self.log.emit("Segmentation Finished")
            self.log.emit("======================")

        except Exception as ex:
            self.error.emit(str(ex))

class MainWindow(QWidget):

    def append_log(self, text):
        self.log.append(text)
        self.log.moveCursor(QTextCursor.End)

    def item_changed(self, item, column):

        if self.updating:
            return

        self.updating = True

        #
        # Parent clicked
        #

        if item.childCount() > 0:

            state = item.checkState(0)

            for i in range(item.childCount()):

                item.child(i).setCheckState(0, state)

        #
        # Child clicked
        #

        else:

            parent = item.parent()

            checked = 0

            for i in range(parent.childCount()):

                if parent.child(i).checkState(0) == Qt.Checked:

                    checked += 1

            if checked == 0:

                parent.setCheckState(0, Qt.Unchecked)

            elif checked == parent.childCount():

                parent.setCheckState(0, Qt.Checked)

            else:

                parent.setCheckState(0, Qt.PartiallyChecked)

        self.updating = False

    def __init__(self):

        super().__init__()
        

        self.setWindowTitle("AI Organ Segmentation")

        self.resize(500, 650)

        
        self.build_ui()

    # ------------------------------------------------

    def build_ui(self):

        layout = QVBoxLayout(self)

        # CT Folder

        layout.addWidget(QLabel("CT Folder"))

        ctLayout = QHBoxLayout()

        self.ctEdit = QLineEdit()

        self.ctEdit.setText(
            os.path.join(
                os.path.dirname(__file__),
                "Input"
            )
        )

        browseCT = QPushButton("Browse")

        browseCT.clicked.connect(self.browse_ct)

        ctLayout.addWidget(self.ctEdit)

        ctLayout.addWidget(browseCT)

        layout.addLayout(ctLayout)

        # Output Folder

        layout.addWidget(QLabel("Output Folder"))

        outLayout = QHBoxLayout()

        self.outputEdit = QLineEdit()

        self.outputEdit.setText(
            os.path.join(
                os.path.dirname(__file__),
                "Output"
            )
        )

        browseOut = QPushButton("Browse")

        browseOut.clicked.connect(self.browse_output)

        outLayout.addWidget(self.outputEdit)

        outLayout.addWidget(browseOut)

        layout.addLayout(outLayout)

        # Structures
        layout.addWidget(QLabel("Structures"))

        self.tree = QTreeWidget()

        self.tree.setHeaderHidden(True)

        layout.addWidget(self.tree)
        
        for region, structures in REGIONS.items():

            regionItem = QTreeWidgetItem(self.tree)

            regionItem.setText(0, region)

            regionItem.setFlags(
                regionItem.flags() |
                Qt.ItemIsUserCheckable
            )

            regionItem.setCheckState(0, Qt.Unchecked)

            for name in structures:

                child = QTreeWidgetItem(regionItem)

                child.setText(0, name)

                child.setFlags(
                    child.flags() |
                    Qt.ItemIsUserCheckable
                )

                child.setCheckState(0, Qt.Unchecked)

        self.tree.collapseAll()

        self.tree.itemChanged.connect(self.item_changed)

        self.updating = False

        # Buttons

        buttonLayout = QHBoxLayout()

        selectAll = QPushButton("Select All")

        clearAll = QPushButton("Clear All")

        selectAll.clicked.connect(self.select_all)

        clearAll.clicked.connect(self.clear_all)

        buttonLayout.addWidget(selectAll)

        buttonLayout.addWidget(clearAll)

        layout.addLayout(buttonLayout)

        # Predict

        self.predictButton = QPushButton("Predict")

        self.predictButton.setMinimumHeight(40)

        self.predictButton.clicked.connect(self.predict)

        layout.addWidget(self.predictButton)

        # Status

        layout.addWidget(QLabel("Status"))

        self.status = QLabel("Waiting")

        layout.addWidget(self.status)

        self.progress = QProgressBar()

        self.progress.setValue(0)

        layout.addWidget(self.progress)

        self.gpuCheck = QCheckBox(
            "Use GPU acceleration"
        )

        layout.addWidget(self.gpuCheck)

        layout.addWidget(QLabel("Log"))

        self.log = QTextEdit()
        self.log.setReadOnly(True)
        self.log.setLineWrapMode(QTextEdit.NoWrap)

        layout.addWidget(self.log)

    # ------------------------------------------------

    def browse_ct(self):

        folder = QFileDialog.getExistingDirectory(self)

        if folder:

            self.ctEdit.setText(folder)

    # ------------------------------------------------

    def browse_output(self):

        folder = QFileDialog.getExistingDirectory(self)

        if folder:

            self.outputEdit.setText(folder)

    # ------------------------------------------------


    def worker_finished(self):

        self.status.setText("Finished")

        self.predictButton.setEnabled(True)

        QMessageBox.information(
            self,
            "Done",
            "Segmentation Finished."
        )
        self.progress.setValue(100)
        self.status.setText("Finished")


    def worker_error(self, message):

        self.status.setText("Error")

        self.predictButton.setEnabled(True)
        self.progress.setValue(0)

        QMessageBox.critical(
            self,
            "Error",
            message
        )

    # ------------------------------------------------


    def select_all(self):

        root = self.tree.invisibleRootItem()

        for i in range(root.childCount()):

            root.child(i).setCheckState(0, Qt.Checked)

    def clear_all(self):

        root = self.tree.invisibleRootItem()

        for i in range(root.childCount()):

            root.child(i).setCheckState(0, Qt.Unchecked)
            
    def predict(self):
        self.log.clear()

        self.progress.setValue(0)

        self.predictButton.setEnabled(False)

        ct_folder = self.ctEdit.text()

        output_folder = self.outputEdit.text()

        if not os.path.exists(ct_folder):

            QMessageBox.critical(
                self,
                "Error",
                "CT Folder not found."
            )

            self.predictButton.setEnabled(True)
            return

        os.makedirs(output_folder, exist_ok=True)


        #
        # Build task groups
        #

        tasks = {}

        root = self.tree.invisibleRootItem()

        for i in range(root.childCount()):

            region = root.child(i)

            for j in range(region.childCount()):

                item = region.child(j)

                if item.checkState(0) != Qt.Checked:
                    continue

                region_name = region.text(0)
                structure_name = item.text(0)

                info = REGIONS[region_name][structure_name]

                task = info["task"]
                roi = info["roi"]

                tasks.setdefault(task, [])

                if roi is not None:
                    tasks[task].append(roi)
        #
        # Run every task
        #
        if not tasks:
            QMessageBox.warning(
                self,
                "No Structures",
                "Please select at least one structure."
            )
            self.predictButton.setEnabled(True)
            
            return
        self.log.append(str(tasks))

        self.thread = QThread()

        self.worker = Worker(
            ct_folder,
            output_folder,
            tasks,
            self.gpuCheck.isChecked()
        )
        self.worker.progress.connect(
            self.progress.setValue
        )

        self.worker.moveToThread(self.thread)

        self.thread.started.connect(self.worker.run)

        self.worker.status.connect(self.status.setText)

        self.worker.log.connect(self.append_log)

        self.worker.finished.connect(self.worker_finished)

        self.worker.error.connect(self.worker_error)

        self.worker.error.connect(self.thread.quit)
        self.worker.error.connect(self.worker.deleteLater)

        self.worker.finished.connect(self.thread.quit)

        self.worker.finished.connect(self.worker.deleteLater)

        self.thread.finished.connect(self.thread.deleteLater)

        self.thread.start()
# ----------------------------------------------------

if __name__ == "__main__":

    app = QApplication(sys.argv)

    window = MainWindow()

    window.show()

    sys.exit(app.exec())
