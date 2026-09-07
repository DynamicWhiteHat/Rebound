# Data Processing
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Modelling
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, confusion_matrix, ConfusionMatrixDisplay, f1_score, classification_report
from sklearn.model_selection import GroupKFold


data = pd.read_csv("final_master.csv")
x = data.drop(columns=["id", "label", "trial_type"], errors='ignore')
y = data["label"]
groups = data["id"]


# Find the number of unique patients for Leave-One-Subject-Out Cross Validation
num_patients = len(groups.unique())
gkf = GroupKFold(n_splits=num_patients)

# Lists to track performance across all folds
fold_accuracies = []
fold_f1s = []
true_labels = []
predicted_labels = []

print(f"Starting Leave-One-Subject-Out Cross Validation across {num_patients} patients...\n")

# 3. REPLACE TRAIN_TEST_SPLIT WITH THE GROUP SPLITTING LOOP
for fold, (train_idx, test_idx) in enumerate(gkf.split(x, y, groups=groups)):
    # Slice the rows safely based on patient groupings
    x_train, x_test = x.iloc[train_idx], x.iloc[test_idx]
    y_train, y_test = y.iloc[train_idx], y.iloc[test_idx]
    
    # Identify the patient currently sitting in the testing pool
    test_patient = groups.iloc[test_idx].unique()[0]
    
    # Train the Random Forest Classifier
    rf = RandomForestClassifier(class_weight='balanced', n_jobs=-1, random_state=42, max_depth=18, max_features='sqrt', min_samples_leaf=8, min_samples_split=6, n_estimators=472)

    rf.fit(x_train, y_train)
    
    # Evaluate
    y_pred = rf.predict(x_test)
    true_labels.extend(y_test)
    predicted_labels.extend(y_pred)
    
    accuracy = accuracy_score(y_test, y_pred)
    fold_accuracies.append(accuracy)
    macro_f1 = f1_score(y_test, y_pred, average='macro')
    fold_f1s.append(macro_f1)
    print(f"Fold {fold+1}: Test Subject = {test_patient} | Accuracy: {accuracy:.4f} | Macro F1: {macro_f1:.4f}")

mean_accuracy = np.mean(fold_accuracies)
print(f"\nFinal Overall Generalized Accuracy: {mean_accuracy:.4f}")
print(classification_report(true_labels, predicted_labels, target_names=['Null', 'Chewing', 'Swallow', 'Cough', 'Speech']))
# --- BUG FIX 1: DYNAMIC CONFUSION MATRIX LABELS ---
cm = confusion_matrix(true_labels, predicted_labels)
# Pull only the unique classes that actually exist in your dataset
unique_classes = np.unique(true_labels) 
disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=unique_classes)
disp.plot(cmap='Blues')

plt.show()
