import os
import sys
import numpy as np
import SimpleITK as sitk
from rt_utils import RTStructBuilder


ROI_COLORS = {

    "Brain":               [255,255,0],
    "BrainStem":          [128,128,0],
    "Cerebellum":          [255,0,255],
    "Thalamus":          [220,220,220],
    "Frontal Lobe":    [110,110,110],
    "Parietal Lobe":    [150,150,150],
    "Septum Pellucidum":    [101,100,100],
    "Temporal Lobe":    [100,100,100],
    "Occipital Lobe":    [151,100,100],
    "Ventricle":    [120,120,120],

    "Breast":       [106,137,241],
    
    "Heart":               [150,75,0],

    "Lung_L":           [0,0,255],
    "Lung_R":          [0,100,0],

    "SpinalCord":         [255,128,128],

    "Liver":               [255, 255, 237],

    "Kidney_L":         [0,255,0],
    "Kidney_R":        [0,180,0],

    "Thyroid":             [255,200,0],

    "Esophagus":           [255,196,255],

    "Trachea":             [000,191,191],

    "UrinaryBladder":     [255,255,120],

    "Colon":               [255,170,170],

    "SmallBowel":         [170,255,170],

    "Sacrum":              [255,255,255],

    "Cervical_Vertebrae":           [255,105,180],
    "Thoracic_Vertebrae":           [225,110,180],
    "Lumbar_Vertebrae":           [210,115,180],

    "Pancreas":           [255,174,66],
    "Prostate":           [255,0,0],
    "Femur_L":          [128,10,10],
    "Femur_R":          [128,125,110],
    "Stomach":              [192,194,194],
    "Spleen": [221,255,255]
    

    "Eye_L":           [0,1,39],
    "Eye_R":           [0,64,0],
    "Lens_L":           [144, 238, 144],
    "Lens_R":           [255,192,191],
    "OpticNerve_L":           [200,180,255],
    "OpticNerve_R":           [128,0,128],

    "Parotid_L":  [130,255,100],
    "Parotid_R": [167,167,167],

    "Larynx_Air": [192,100,102]
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

    "breast" : "Breast",
    "parotid_gland_left" : "Parotid_L",
    "parotid_gland_right" : "Parotid_R",
    "spleen": "Spleen"
}

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
    ],

}

def main():

    if len(sys.argv) != 3:
        print("Usage:")
        print("python convert.py <CT_FOLDER> <MASK_FOLDER>")
        sys.exit(1)

    CT_FOLDER = sys.argv[1]
    MASK_FOLDER = sys.argv[2]
    OUTPUT_FILE = os.path.join(MASK_FOLDER, "RS.dcm")

    merged_masks = {
        name: None
        for name in MERGED_ROIS
    }

    print("Loading CT series...")

    rtstruct = RTStructBuilder.create_new(
        dicom_series_path=CT_FOLDER
    )
    

    #
    # -------------------------------------------------------
    # Find masks
    # -------------------------------------------------------
    #

    mask_files = sorted(
        f for f in os.listdir(MASK_FOLDER)
        if f.endswith(".nii.gz")
    )

    print(f"Found {len(mask_files)} mask(s).")

    for filename in mask_files:

        path = os.path.join(MASK_FOLDER, filename)

        mask_name = filename.replace(".nii.gz", "")

        img = sitk.ReadImage(path)

        mask = sitk.GetArrayFromImage(img)

        mask = np.transpose(mask, (1,2,0))
        mask = np.flip(mask, axis=0)
        mask = mask.astype(bool)

        if not np.any(mask):
            continue

        merged = False

        for merged_name, members in MERGED_ROIS.items():

            if mask_name in members:

                if merged_masks[merged_name] is None:
                    merged_masks[merged_name] = mask.copy()
                else:
                    merged_masks[merged_name] |= mask

                merged = True
                break

        if merged:
            continue

        roi_name = DISPLAY_NAMES.get(mask_name, mask_name)

        color = ROI_COLORS.get(
            roi_name,
            [255,255,255]
        )

        print(f"Adding {roi_name}")

        rtstruct.add_roi(
            mask=mask,
            name=roi_name,
            color=color
        )

    #
    # -------------------------------------------------------
    # Add merged ROIs
    # -------------------------------------------------------
    #
    for roi_name, mask in merged_masks.items():

        if mask is None:
            continue

        if not np.any(mask):
            continue

        print(f"Adding {roi_name}")

        color = ROI_COLORS.get(
            roi_name,
            [255,255,255]
        )

        rtstruct.add_roi(
            mask=mask,
            name=roi_name,
            color=color
        )

    print("Saving RTSTRUCT...")

    rtstruct.save(OUTPUT_FILE)

    print(f"Done. Saved to {OUTPUT_FILE}")


if __name__ == "__main__":
    main()
