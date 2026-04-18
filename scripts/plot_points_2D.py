import csv
import os
import sys

import matplotlib.pyplot as plt


def load_points(filename):
    points_x = []
    points_y = []

    with open(filename, "r", newline="") as file:
        reader = csv.reader(file)
        first_row = True

        for row in reader:
            if not row:
                continue

            if first_row:
                first_row = False
                try:
                    float(row[0])
                    float(row[1])
                except ValueError:
                    continue

            if len(row) < 2:
                continue

            try:
                x = float(row[0])
                y = float(row[1])
            except ValueError:
                continue

            points_x.append(x)
            points_y.append(y)

    return points_x, points_y


def make_output_filename(input_filename):
    os.makedirs("Results/PNG", exist_ok=True)
    base = os.path.splitext(os.path.basename(input_filename))[0]
    return os.path.join("Results", "PNG", f"{base}_points_2D.png")

def main():
    if len(sys.argv) != 2:
        print("Использование: python3 scripts/plot_points_2D.py <points_csv>")
        sys.exit(1)

    input_filename = sys.argv[1]
    points_x, points_y = load_points(input_filename)

    if not points_x:
        print("Не удалось загрузить точки из файла")
        sys.exit(1)

    output_filename = make_output_filename(input_filename)

    plt.figure(figsize=(8, 6))
    plt.scatter(points_x, points_y, s=5, alpha=0.6)
    plt.title(f"2D-визуализация точек (ось X-Y)\nВсего точек: {len(points_x)}")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_filename, dpi=200)
    plt.close()

    print(f"Изображение сохранено в {output_filename}")


if __name__ == "__main__":
    main()