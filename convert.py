python
import os
import sys
import math
import numpy as np
import pydicom	
import SimpleITK as sitk

from rt_utils import RTStructBuilder



# ============================================================
# CONFIGURATION
# ============================================================

ROI_COLORS = {

    "Brain":               [255, 255, 0],
    "BrainStem":           [128, 128, 0],
    "Cerebellum":           [255, 0, 255],
    "Thalamus":             [220, 220, 220],
    "Frontal Lobe":         [110, 110, 110],
    "Parietal Lobe":        [150, 150, 150],
    "Septum Pellucidum":    [101, 100, 100],
    "Temporal Lobe":        [100, 100, 100],
    "Occipital Lobe":       [151, 100, 100],
    "Ventricle":            [120, 120, 120],

    "Breast":               [106, 137, 241],

    "Heart":                [150, 75, 0],

    "Lung_L":               [0, 0, 255],
    "Lung_R":               [0, 100, 0],

    "SpinalCord":           [255, 128, 128],

    "Liver":                [255, 255, 237],

    "Kidney_L":             [0, 255, 0],
    "Kidney_R":             [0, 180, 0],

    "Thyroid":              [255, 200, 0],

    "Esophagus":            [255, 196, 255],

    "Trachea":              [0, 191, 191],

    "UrinaryBladder":       [255, 255, 120],

    "Colon":                [255, 170, 170],

    "SmallBowel":           [170, 255, 170],

    "Sacrum":               [255, 255, 255],

    "Cervical_Vertebrae":   [255, 105, 180],
    "Thoracic_Vertebrae":   [225, 110, 180],
    "Lumbar_Vertebrae":     [210, 115, 180],

    "Pancreas":             [255, 174, 66],
    "Prostate":             [255, 0, 0],

    "Femur_L":              [128, 10, 10],
    "Femur_R":              [128, 125, 110],

    "Stomach":              [192, 194, 194],
    "Spleen":               [221, 255, 255],

    "Eye_L":                [0, 1, 39],
    "Eye_R":                [0, 64, 0],

    "Lens_L":               [144, 238, 144],
    "Lens_R":               [255, 192, 191],

    "OpticNerve_L":         [200, 180, 255],
    "OpticNerve_R":         [128, 0, 128],

    "Parotid_L":            [130, 255, 100],
    "Parotid_R":            [167, 167, 167],

    "Larynx_Air":           [192, 100, 102],
}


DISPLAY_NAMES = {

    "brain": "Brain",
    "brainstem": "BrainStem",
    "cerebellum": "Cerebellum",
    "thalamus": "Thalamus",

    "frontal_lobe": "Frontal Lobe",
    "parietal_lobe": "Parietal Lobe",
    "occipital_lobe": "Occipital Lobe",
    "temporal_lobe": "Temporal Lobe",

    "septum_pellucidum": "Septum Pellucidum",

    "heart": "Heart",

    "spinal_cord": "SpinalCord",

    "liver": "Liver",

    "thyroid_gland": "Thyroid",

    "pancreas": "Pancreas",
    "prostate": "Prostate",

    "femur_left": "Femur_L",
    "femur_right": "Femur_R",

    "stomach": "Stomach",

    "esophagus": "Esophagus",

    "trachea": "Trachea",

    "kidney_left": "Kidney_L",
    "kidney_right": "Kidney_R",

    "urinary_bladder": "UrinaryBladder",

    "colon": "Colon",

    "small_bowel": "SmallBowel",

    "sacrum": "Sacrum",

    "eyeball_left": "Eye_L",
    "eyeball_right": "Eye_R",

    "eye_lens_left": "Lens_L",
    "eye_lens_right": "Lens_R",

    "optic_nerve_left": "OpticNerve_L",
    "optic_nerve_right": "OpticNerve_R",

    "parotid_gland_left": "Parotid_L",
    "parotid_gland_right": "Parotid_R",

    "larynx_air": "Larynx_air",

    "breast": "Breast",

    "spleen": "Spleen",
}


# ============================================================
# BOOLEAN / MERGED STRUCTURES
# ============================================================

MERGED_ROIS = {

    "Lung_L": [
        "lung_upper_lobe_left",
        "lung_lower_lobe_left",
    ],

    "Lung_R": [
        "lung_upper_lobe_right",
        "lung_middle_lobe_right",
        "lung_lower_lobe_right",
    ],

    "Cervical_Vertebrae": [
        "vertebrae_C1",
        "vertebrae_C2",
        "vertebrae_C3",
        "vertebrae_C4",
        "vertebrae_C5",
        "vertebrae_C6",
        "vertebrae_C7",
    ],

    "Thoracic_Vertebrae": [
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
    ],

    "Lumbar_Vertebrae": [
        "vertebrae_L1",
        "vertebrae_L2",
        "vertebrae_L3",
        "vertebrae_L4",
        "vertebrae_L5",
        "sacrum",
    ],
}


# ============================================================
# DICOM DECIMAL STRING SAFETY
# ============================================================

def dicom_ds(value):
    """
    Convert a floating-point coordinate into a DICOM DS-safe
    decimal string.

    DICOM DS allows a maximum of 16 characters.

    We use up to 6 decimal places, which is vastly more
    precision than clinically meaningful for CT contour
    coordinates while remaining safely within the DS limit.
    """

    value = float(value)

    if not math.isfinite(value):
        raise ValueError(
            f"Non-finite contour coordinate encountered: {value}"
        )

    # Avoid negative zero
    if abs(value) < 0.0000005:
        value = 0.0

    text = f"{value:.6f}".rstrip("0").rstrip(".")

    # Safety check
    if len(text) > 16:

        # Fall back to scientific notation
        text = f"{value:.8E}"

    if len(text) > 16:
        raise ValueError(
            f"Could not create a valid DICOM DS value "
            f"from coordinate {value}: '{text}'"
        )

    return text


# ============================================================
# CT SERIES LOADING
# ============================================================
def sanitize_rtstruct_contours(path):
    ds = pydicom.dcmread(path)

    if "ROIContourSequence" not in ds:
        return

    for roi_contour in ds.ROIContourSequence:

        if "ContourSequence" not in roi_contour:
            continue

        for contour in roi_contour.ContourSequence:

            if "ContourData" not in contour:
                continue

            contour.ContourData = [
                dicom_ds(value)
                for value in contour.ContourData
            ]

    ds.save_as(path)

    print("Contour DS values sanitized.")

def load_ct_series(ct_folder):

    print()
    print("==========================================")
    print("Loading original CT series")
    print("==========================================")

    reader = sitk.ImageSeriesReader()

    series_ids = reader.GetGDCMSeriesIDs(ct_folder)

    if not series_ids:
        raise RuntimeError(
            f"No DICOM series found in:\n{ct_folder}"
        )

    if len(series_ids) > 1:

        print(
            f"WARNING: Found {len(series_ids)} DICOM series."
        )

        print("Series IDs:")

        for series_id in series_ids:
            print("  ", series_id)

        # Select the series with the most files
        selected_series = max(
            series_ids,
            key=lambda sid: len(
                reader.GetGDCMSeriesFileNames(
                    ct_folder,
                    sid
                )
            )
        )

        print(
            "Selected series with most slices:"
        )
        print(selected_series)

    else:
        selected_series = series_ids[0]

    file_names = reader.GetGDCMSeriesFileNames(
        ct_folder,
        selected_series
    )

    if not file_names:
        raise RuntimeError(
            "Could not obtain DICOM files for CT series."
        )

    reader.SetFileNames(file_names)

    ct = reader.Execute()

    print("CT size:      ", ct.GetSize())
    print("CT spacing:   ", ct.GetSpacing())
    print("CT origin:    ", ct.GetOrigin())
    print("CT direction: ", ct.GetDirection())

    return ct


# ============================================================
# GEOMETRY COMPARISON
# ============================================================

def geometry_matches(mask_img, ct_img, tolerance=1e-4):

    mask_size = np.array(mask_img.GetSize())
    ct_size = np.array(ct_img.GetSize())

    if not np.array_equal(mask_size, ct_size):
        return False

    mask_spacing = np.array(mask_img.GetSpacing())
    ct_spacing = np.array(ct_img.GetSpacing())

    if not np.allclose(
        mask_spacing,
        ct_spacing,
        atol=tolerance,
        rtol=0
    ):
        return False

    mask_origin = np.array(mask_img.GetOrigin())
    ct_origin = np.array(ct_img.GetOrigin())

    if not np.allclose(
        mask_origin,
        ct_origin,
        atol=tolerance,
        rtol=0
    ):
        return False

    mask_direction = np.array(
        mask_img.GetDirection()
    )

    ct_direction = np.array(
        ct_img.GetDirection()
    )

    if not np.allclose(
        mask_direction,
        ct_direction,
        atol=tolerance,
        rtol=0
    ):
        return False

    return True


# ============================================================
# RESAMPLE MASK TO CT GRID
# ============================================================

def resample_mask_to_ct(mask_img, ct_img):

    print(
        "Geometry mismatch detected."
    )

    print(
        "Resampling segmentation onto EXACT CT grid..."
    )

    resampler = sitk.ResampleImageFilter()

    resampler.SetReferenceImage(ct_img)

    # IMPORTANT:
    # Segmentation masks must use nearest-neighbour.
    # Never use linear/cubic interpolation for labels.
    resampler.SetInterpolator(
        sitk.sitkNearestNeighbor
    )

    resampler.SetDefaultPixelValue(0)

    resampler.SetTransform(
        sitk.Transform()
    )

    resampled = resampler.Execute(mask_img)

    return resampled


# ============================================================
# CONVERT SITK MASK TO RT-UTILS ORIENTATION
# ============================================================

def sitk_mask_to_rtutils(mask_img):

    """
    SimpleITK array:

        [z, y, x]

    rt-utils expects:

        [x, y, z]

    with the orientation corresponding to the DICOM
    image series.

    This preserves the orientation convention used by the
    existing converter.
    """

    mask = sitk.GetArrayFromImage(
        mask_img
    )

    if mask.ndim != 3:
        raise ValueError(
            f"Expected 3D mask, got shape {mask.shape}"
        )

    mask = np.transpose(
        mask,
        (1, 2, 0)
    )

    mask = np.flip(
        mask,
        axis=0
    )

    mask = mask.astype(bool)

    return mask


# ============================================================
# VALIDATE MASK
# ============================================================

def validate_mask(mask, ct_img, roi_name):

    expected_size = (
        ct_img.GetSize()[0],
        ct_img.GetSize()[1],
        ct_img.GetSize()[2],
    )

    if mask.shape != expected_size:

        raise ValueError(
            f"Mask geometry error for '{roi_name}'.\n"
            f"Expected: {expected_size}\n"
            f"Got:      {mask.shape}"
        )

    if mask.dtype != np.bool_:

        mask = mask.astype(bool)

    if not np.any(mask):

        print(
            f"WARNING: {roi_name} mask is empty."
        )

    return mask


# ============================================================
# MAIN
# ============================================================

def main():

    if len(sys.argv) != 3:

        print()
        print("Usage:")
        print(
            "python convert.py <CT_FOLDER> <MASK_FOLDER>"
        )
        print()

        sys.exit(1)

    CT_FOLDER = os.path.abspath(
        sys.argv[1]
    )

    MASK_FOLDER = os.path.abspath(
        sys.argv[2]
    )

    OUTPUT_FILE = os.path.join(
        MASK_FOLDER,
        "RS.dcm"
    )

    if not os.path.isdir(CT_FOLDER):

        raise RuntimeError(
            f"CT folder does not exist:\n{CT_FOLDER}"
        )

    if not os.path.isdir(MASK_FOLDER):

        raise RuntimeError(
            f"Mask folder does not exist:\n{MASK_FOLDER}"
        )

    print()
    print("==========================================")
    print("AI SEGMENTATION → RTSTRUCT")
    print("==========================================")

    print()
    print("CT folder:")
    print(CT_FOLDER)

    print()
    print("Mask folder:")
    print(MASK_FOLDER)

    # --------------------------------------------------------
    # 1. Load ORIGINAL CT geometry
    # --------------------------------------------------------

    ct_img = load_ct_series(
        CT_FOLDER
    )

    # --------------------------------------------------------
    # 2. Create RTSTRUCT using ORIGINAL CT series
    # --------------------------------------------------------

    print()
    print("Creating RTSTRUCT from original CT series...")

    rtstruct = RTStructBuilder.create_new(
        dicom_series_path=CT_FOLDER
    )

    # --------------------------------------------------------
    # 3. Prepare merged masks
    # --------------------------------------------------------

    merged_masks = {
        name: None
        for name in MERGED_ROIS
    }

    # --------------------------------------------------------
    # 4. Find NIfTI masks
    # --------------------------------------------------------

    mask_files = sorted(
        f
        for f in os.listdir(MASK_FOLDER)
        if f.endswith(".nii.gz")
    )

    print()
    print(
        f"Found {len(mask_files)} NIfTI mask(s)."
    )

    if not mask_files:

        raise RuntimeError(
            "No .nii.gz masks were found."
        )

    # --------------------------------------------------------
    # 5. Process every mask
    # --------------------------------------------------------

    for filename in mask_files:

        mask_path = os.path.join(
            MASK_FOLDER,
            filename
        )

        mask_name = filename[:-7]

        print()
        print("------------------------------------------")
        print(f"Processing: {mask_name}")
        print("------------------------------------------")

        # ----------------------------------------------------
        # Read NIfTI
        # ----------------------------------------------------

        mask_img = sitk.ReadImage(
            mask_path
        )

        print(
            "NIfTI size:      ",
            mask_img.GetSize()
        )

        print(
            "NIfTI spacing:   ",
            mask_img.GetSpacing()
        )

        print(
            "NIfTI origin:    ",
            mask_img.GetOrigin()
        )

        # ----------------------------------------------------
        # Verify geometry
        # ----------------------------------------------------

        matches = geometry_matches(
            mask_img,
            ct_img
        )

        if matches:

            print(
                "Geometry: EXACT MATCH"
            )

        else:

            print(
                "Geometry: MISMATCH"
            )

            mask_img = resample_mask_to_ct(
                mask_img,
                ct_img
            )

            # Verify again
            if not geometry_matches(
                mask_img,
                ct_img
            ):

                raise RuntimeError(
                    f"Failed to align mask '{mask_name}' "
                    "to CT geometry."
                )

            print(
                "Geometry: RESAMPLED TO CT GRID"
            )

        # ----------------------------------------------------
        # Convert to RT-utils orientation
        # ----------------------------------------------------

        mask = sitk_mask_to_rtutils(
            mask_img
        )

        # ----------------------------------------------------
        # Validate
        # ----------------------------------------------------

        roi_name = DISPLAY_NAMES.get(
            mask_name,
            mask_name
        )

        mask = validate_mask(
            mask,
            ct_img,
            roi_name
        )

        if not np.any(mask):

            print(
                f"Skipping empty ROI: {roi_name}"
            )

            continue

        # ----------------------------------------------------
        # Check whether this is a merged structure
        # ----------------------------------------------------

        merged = False

        for merged_name, members in MERGED_ROIS.items():

            if mask_name in members:

                print(
                    f"Adding mask to merged ROI: "
                    f"{merged_name}"
                )

                if merged_masks[
                    merged_name
                ] is None:

                    merged_masks[
                        merged_name
                    ] = mask.copy()

                else:

                    merged_masks[
                        merged_name
                    ] |= mask

                merged = True

                break

        if merged:
            continue

        # ----------------------------------------------------
        # Normal ROI
        # ----------------------------------------------------

        color = ROI_COLORS.get(
            roi_name,
            [255, 255, 255]
        )

        print(
            f"Adding ROI: {roi_name}"
        )

        rtstruct.add_roi(
            mask=mask,
            name=roi_name,
            color=color
        )

    # --------------------------------------------------------
    # 6. Add merged ROIs
    # --------------------------------------------------------

    print()
    print("==========================================")
    print("Adding merged ROIs")
    print("==========================================")

    for roi_name, mask in merged_masks.items():

        if mask is None:
            continue

        if not np.any(mask):
            continue

        color = ROI_COLORS.get(
            roi_name,
            [255, 255, 255]
        )

        print(
            f"Adding merged ROI: {roi_name}"
        )

        rtstruct.add_roi(
            mask=mask,
            name=roi_name,
            color=color
        )

    # --------------------------------------------------------
    # 7. Save
    # --------------------------------------------------------

    print()
    print("==========================================")
    print("Saving RTSTRUCT")
    print("==========================================")

    print(
        f"Output: {OUTPUT_FILE}"
    )

    rtstruct.save(
        OUTPUT_FILE
    )

    print("Sanitizing DICOM contour coordinates...")

    sanitize_rtstruct_contours(
        OUTPUT_FILE
    )
    # --------------------------------------------------------
    # 8. Final validation
    # --------------------------------------------------------

    if not os.path.exists(
        OUTPUT_FILE
    ):

        raise RuntimeError(
            "RTSTRUCT file was not created."
        )

    file_size = os.path.getsize(
        OUTPUT_FILE
    )

    print()
    print("==========================================")
    print("SUCCESS")
    print("==========================================")

    print(
        f"RTSTRUCT size: "
        f"{file_size / 1024:.1f} KB"
    )

    print(
        f"Saved to:\n{OUTPUT_FILE}"
    )


if __name__ == "__main__":
    main()
