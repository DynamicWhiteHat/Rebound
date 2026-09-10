# Data Processing
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Modelling
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import accuracy_score, confusion_matrix, ConfusionMatrixDisplay, f1_score, classification_report
from sklearn.model_selection import GroupKFold

# Utilities
import os
from datetime import datetime

data = pd.read_csv("final_master.csv")
label_map = {0: 0, 1: 1, 2: 2, 3: 0, 4: 0}
data['label'] = data['label'].map(label_map)

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
print(classification_report(true_labels, predicted_labels, target_names=['Null/Cough/Speech', 'Chewing', 'Swallowing']))
print(f"Feature Importances:")
for i, importance in enumerate(rf.feature_importances_):
    print(f"  {x.columns[i]}: {importance:.4f}")

today_str = datetime.now().strftime("%Y-%m-%d")
output_dir = os.path.join("Results", today_str)
os.makedirs(output_dir, exist_ok=True)

run_num = 1
while os.path.exists(os.path.join(output_dir, f"confusion_matrix_{run_num}.png")):
    run_num += 1

save_path = os.path.join(output_dir, f"confusion_matrix_{run_num}.png")

cm = confusion_matrix(true_labels, predicted_labels)
unique_classes = np.unique(true_labels) 
disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=unique_classes)
disp.plot(cmap='Blues')

plt.savefig(save_path, dpi=300, bbox_inches='tight')
print(f"Confusion matrix saved to: {save_path}")

plt.show()

