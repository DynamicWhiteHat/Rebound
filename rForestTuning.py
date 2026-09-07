from scipy.stats import randint
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import GroupKFold, RandomizedSearchCV
import pandas as pd

data = pd.read_csv("final_master.csv")
x = data.drop(columns=["id", "label", "trial_type"], errors='ignore')
y = data["label"]
groups = data["id"]


num_patients = len(groups.unique())
gkf = GroupKFold(n_splits=num_patients)

param_dist = {
    'n_estimators': randint(100, 500),
    'max_depth': randint(3, 20),
    'min_samples_split': randint(2, 10),
    'min_samples_leaf': randint(1, 10),
    'max_features': ['sqrt', 'log2', None]
}

search = RandomizedSearchCV(
    estimator=RandomForestClassifier(class_weight='balanced', n_jobs=-1, random_state=42),
    param_distributions=param_dist,
    n_iter=100,
    cv=gkf,
    scoring='f1_macro',
    random_state=42
)

search.fit(x, y, groups=groups)
print("Best parameters found:", search.best_params_)