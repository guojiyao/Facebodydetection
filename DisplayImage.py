import cv2
import sys
import matplotlib.pyplot as plt
img = cv2.imread(sys.argv[1])

plt.imshow(img)
plt.show()
