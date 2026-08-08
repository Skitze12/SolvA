import os
import sys
import numpy as np
import SimpleITK as sitk
from rt_utils import RTStructBuilder


# ============================================================
# COLORS
# ============================================================

ROI_COLORS = {

    "Brain":               [255,255,0],
    "BrainStem":           [128,128,0],
    "Cerebellum":           [255,0,255],
    "Thalamus":             [220,220,220],
    "Frontal Lobe":         [110,110,110],
    "Parietal Lobe":        [150,150,150],
    "Septum Pellucidum":    [101,100,100],
    "Temporal Lobe":        [100,100,100],
    "Occipital Lobe":       [151,100,100],
    "Ventricle":            [120,120,120],

    "Breast":               [106,137,241],

    "Heart":                [150,75,0],

    "Lung_L":               [0,0,255],
    "Lung_R":               [0,100,0],

    "SpinalCord":            [255,128,128],

    "Liver":                [255,255,237],

    "Kidney_L":             [0,255,0],
    "Kidney_R":             [0,180,0],

    "Thyroid":              [255,200,0],

    "Esophagus":            [255,196,255],

    "Trachea":              [0,191,191],

    "UrinaryBladder":       [255,255,120],

    "Colon":                [255,170,170],

    "SmallBowel":           [170,255,170],

    "Sacrum":               [255,255,255],

    "Cervical_Vertebrae":   [255,105,180],
    "Thoracic_Vertebrae":   [225,110,180],
    "Lumbar_Vertebrae":     [210,115,180],

    "Pancreas":             [255,174,66],
    "Prostate":             [255,0,0],

    "Femur_L":              [128,10,10],
    "Femur_R":              [128,125,110],

    "Stomach":              [192,194,194],
    "Spleen":               [221,255,255],

    "Eye_L":                [0,1,39],
    "Eye_R":                [0,64,0],

    "Lens_L":               [144,238,144],
    "Lens_R":               [255,192,191],

    "OpticNerve_L":         [200,180,255],
    "OpticNerve_R":         [128,0,128],

    "Parotid_L":            [130,255,100],
    "Parotid_R":            [167,167,167],

    "Larynx_Air":           [192,100,102]
}


# ============================================================
# DISPLAY NAMES
# ============================================================

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

    "spleen": "Spleen"
}


# ============================================================
# MERGED ROIS
# ============================================================

MERGED_ROIS = {

    "Lung_L": [
        "lung_upper_lobe_left",
        "lung_lower_lobe_left"
    ],

    "Lung_R": [
        "lung_upper_lobe_right",
        "lung_middle_lobe_right",
        "lung_lower_lobe_right"
    ],

    "Cervical_Vertebrae": [
        "vertebrae_C1",
        "vertebrae_C2",
        "vertebrae_C3",
        "vertebrae_C4",
        "vertebrae_C5",
        "vertebrae_C6",
        "vertebrae_C7"
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
        "vertebrae_T12"
    ],

    "Lumbar_Vertebrae": [
        "vertebrae_L1",
        "vertebrae_L2",
        "vertebrae_L3",
        "vertebrae_L4",
        "vertebrae_L5",
        "sacrum"
    ]
}


# ============================================================
# LOAD CT SERIES
# ============================================================

def load_ct_series(ct_folder):

    print("Loading original CT DICOM series...")

    reader = sitk.ImageSeriesReader()

    series_ids = reader.GetGDCMSeriesIDs(ct_folder)

    if not series_ids:
        raise RuntimeError(
            "No DICOM CT series found in:\n" + ct_folder
        )

    if len(series_ids) > 1:

        print(
            f"WARNING: Found {len(series_ids)} DICOM series."
        )

        # Select the largest series.
        # Usually this corresponds to the main CT.
        best_series = None
        best_count = 0

        for series_id in series_ids:

            files = reader.GetGDCMSeriesFileNames(
                ct_folder,
                series_id
            )

            if len(files) > best_count:
                best_count = len(files)
                best_series = series_id

        series_id = best_series

        print(
            f"Using series with {best_count} slices."
        )

    else:

        series_id = series_ids[0]

    dicom_files = reader.GetGDCMSeriesFileNames(
        ct_folder,
        series_id
    )

    if not dicom_files:
        raise RuntimeError(
            "Could not obtain DICOM files from CT series."
        )

    reader.SetFileNames(dicom_files)

    ct = reader.Execute()

    print("CT loaded.")

    print(f"CT size:      {ct.GetSize()}")
    print(f"CT spacing:   {ct.GetSpacing()}")
    print(f"CT origin:    {ct.GetOrigin()}")
    print(f"CT direction: {ct.GetDirection()}")

    return ct


# ============================================================
# GEOMETRY COMPARISON
# ============================================================

def geometry_matches(image, reference):

    return (
        image.GetSize() == reference.GetSize()
        and np.allclose(
            image.GetSpacing(),
            reference.GetSpacing(),
            atol=1e-5
        )
        and np.allclose(
            image.GetOrigin(),
            reference.GetOrigin(),
            atol=1e-3
        )
        and np.allclose(
            image.GetDirection(),
            reference.GetDirection(),
            atol=1e-5
        )
    )


# ============================================================
# RESAMPLE MASK TO CT GRID
# ============================================================

def resample_mask_to_ct(mask_image, ct_image):

    print("Checking mask geometry...")

    if geometry_matches(mask_image, ct_image):

        print("Geometry already matches CT.")

        return mask_image

    print("Mask geometry does NOT match CT.")

    print("Resampling mask onto exact CT grid...")

    resampled = sitk.Resample(
        mask_image,
        ct_image,
        sitk.Transform(),
        sitk.sitkNearestNeighbor,
        0,
        sitk.sitkUInt8
    )

    print("Mask successfully resampled.")

    print(f"New mask size:      {resampled.GetSize()}")
    print(f"New mask spacing:   {resampled.GetSpacing()}")
    print(f"New mask origin:    {resampled.GetOrigin()}")
    print(f"New mask direction: {resampled.GetDirection()}")

    return resampled


# ============================================================
# CONVERT SIMPLEITK MASK TO RT-UTILS ARRAY
# ============================================================

def mask_to_rtutils_array(mask_image):

    mask = sitk.GetArrayFromImage(mask_image)

    # SimpleITK NumPy ordering:
    #
    #   Z, Y, X
    #
    # rt-utils expects:
    #
    #   Y, X, Z
    #
    mask = np.transpose(
        mask,
        (1, 2, 0)
    )

    # Preserve your existing orientation conversion.
    #
    # This is intentionally kept because your current
    # pipeline already produces correctly positioned structures.
    #
    mask = np.flip(
        mask,
        axis=0
    )

    mask = mask.astype(bool)

    return mask


# ============================================================
# MAIN
# ============================================================

def main():

    if len(sys.argv) != 3:

        print("Usage:")
        print("python convert.py <CT_FOLDER> <MASK_FOLDER>")

        sys.exit(1)

    CT_FOLDER = sys.argv[1]

    MASK_FOLDER = sys.argv[2]

    OUTPUT_FILE = os.path.join(
        MASK_FOLDER,
        "RS.dcm"
    )

    # --------------------------------------------------------
    # Load CT geometry
    # --------------------------------------------------------

    ct_image = load_ct_series(
        CT_FOLDER
    )

    # --------------------------------------------------------
    # Create RTSTRUCT using original DICOM
    # --------------------------------------------------------

    print("Creating RTSTRUCT from original CT...")

    rtstruct = RTStructBuilder.create_new(
        dicom_series_path=CT_FOLDER
    )

    # --------------------------------------------------------
    # Prepare merged masks
    # --------------------------------------------------------

    merged_masks = {
        name: None
        for name in MERGED_ROIS
    }

    # --------------------------------------------------------
    # Find masks
    # --------------------------------------------------------

    mask_files = sorted(
        f
        for f in os.listdir(MASK_FOLDER)
        if f.endswith(".nii.gz")
    )

    print(
        f"Found {len(mask_files)} mask(s)."
    )

    # --------------------------------------------------------
    # Process every mask
    # --------------------------------------------------------

    for filename in mask_files:

        path = os.path.join(
            MASK_FOLDER,
            filename
        )

        mask_name = filename.replace(
            ".nii.gz",
            ""
        )

        print("")
        print("--------------------------------")
        print(f"Processing: {mask_name}")
        print("--------------------------------")

        # ----------------------------------------------------
        # Read NIfTI
        # ----------------------------------------------------

        nifti = sitk.ReadImage(path)

        print(
            f"NIfTI size:      {nifti.GetSize()}"
        )

        print(
            f"NIfTI spacing:   {nifti.GetSpacing()}"
        )

        print(
            f"NIfTI origin:    {nifti.GetOrigin()}"
        )

        # ----------------------------------------------------
        # Resample to CT geometry
        # ----------------------------------------------------

        nifti = resample_mask_to_ct(
            nifti,
            ct_image
        )

        # ----------------------------------------------------
        # Convert to rt-utils orientation
        # ----------------------------------------------------

        mask = mask_to_rtutils_array(
            nifti
        )

        # ----------------------------------------------------
        # Empty mask?
        # ----------------------------------------------------

        if not np.any(mask):

            print(
                f"Skipping empty mask: {mask_name}"
            )

            continue

        # ----------------------------------------------------
        # Check whether this belongs to merged ROI
        # ----------------------------------------------------

        merged = False

        for merged_name, members in MERGED_ROIS.items():

            if mask_name in members:

                print(
                    f"Merging {mask_name} -> {merged_name}"
                )

                if merged_masks[merged_name] is None:

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

        roi_name = DISPLAY_NAMES.get(
            mask_name,
            mask_name
        )

        color = ROI_COLORS.get(
            roi_name,
            [255,255,255]
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
    # Add merged ROIs
    # --------------------------------------------------------

    print("")
    print("==============================")
    print("Adding merged ROIs")
    print("==============================")

    for roi_name, mask in merged_masks.items():

        if mask is None:
            continue

        if not np.any(mask):
            continue

        color = ROI_COLORS.get(
            roi_name,
            [255,255,255]
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
    # Save
    # --------------------------------------------------------

    print("")
    print("==============================")
    print("Saving RTSTRUCT...")
    print("==============================")

    rtstruct.save(
        OUTPUT_FILE
    )

    print("")
    print("==============================")
    print("DONE")
    print("==============================")

    print(
        f"Saved to:\n{OUTPUT_FILE}"
    )


# ============================================================
# ENTRY POINT
# ============================================================

if __name__ == "__main__":

    main()