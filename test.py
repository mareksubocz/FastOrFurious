import pandas as pd
import pickle
import seaborn as sns
import matplotlib.pyplot as plt
from sklearn.tree import DecisionTreeRegressor, plot_tree
from sklearn.model_selection import train_test_split
from sklearn import metrics
from sklearn.neural_network import MLPRegressor
from sklearn.ensemble import RandomForestRegressor


df = pd.read_csv('combined.csv')
print(df)
print(df.columns)
X = df.copy(deep=True)
del X['gas']
del X['rotate']
del X['boost']
X['health'] = 0
X['checkpoint'] = 0
X['lap'] = 0
X['rot'] = 0
X['posy'] = 0
X['posx'] = 0
X['relposx'] = 0
X['relposy'] = 0
X['relvelx'] = 0
X['relvely'] = 0
X['relrot'] = 0
X['enemyHealth'] = 0
y = df[['gas', 'rotate', 'boost']]
y['gas'] = y['gas'].astype('bool')
y['rotate'] = y['rotate'].astype('category')
print(y)
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.3, random_state=1) # 70% training and 30% test

# Create Decision Tree classifer object
# clf = DecisionTreeRegressor(max_depth=5)
clf = RandomForestRegressor()
# clf = MLPRegressor()

# Train Decision Tree Classifer
clf = clf.fit(X_train.values,y_train.values)

#Predict the response for test dataset
y_pred = clf.predict(X_test)

# Model Accuracy, how often is the classifier correct?
print("MSE:",metrics.mean_absolute_error(y_test, y_pred))
# corrMatrix = df.corr()
# sns.heatmap(corrMatrix, annot=True)
# plt.show()
print(y_pred)

# labels = ["posx", "posy", "velx", "vely", "rot", "lap", "checkpoint", "health", "relposx", "relposy", "relvelx", "relvely", "relrot", "enemyHealth", "relCheckpointPosx", "relCheckpointPosy", "relCheckpointRot"]
# plt.figure(figsize=(10,10))
# plot_tree(clf, feature_names=labels, fontsize=7)
# plt.show()

with open('model.pickle', 'wb') as file:
    pickle.dump(clf, file)
