import csv
import os
import sys

import matplotlib.pyplot as plt


def load_points(filename):
    points = []

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

            points.append((x, y))

    return points


def make_output_filename(points_filename):
    os.makedirs("Results/PNG", exist_ok=True)
    base = os.path.splitext(os.path.basename(points_filename))[0]
    return os.path.join("Results", "PNG", f"{base}_range_2D.png")


def main():
    if len(sys.argv) != 3:
        print("Использование: python3 scripts/plot_range_2D.py <points_csv> <range_csv>")
        sys.exit(1)

    points_filename = sys.argv[1]
    range_filename = sys.argv[2]

    all_points = load_points(points_filename)
    range_points = load_points(range_filename)

    if not all_points:
        print("Не удалось загрузить исходные точки")
        sys.exit(1)

    if not range_points:
        print("Не удалось загрузить точки диапазонного поиска")
        sys.exit(1)

    all_x = [point[0] for point in all_points]
    all_y = [point[1] for point in all_points]

    range_x = [point[0] for point in range_points]
    range_y = [point[1] for point in range_points]

    output_filename = make_output_filename(points_filename)

    plt.figure(figsize=(8, 6))
    plt.scatter(all_x, all_y, s=4, alpha=0.25, label="Все точки")
    plt.scatter(
        range_x, range_y,
        s=36,
        alpha=0.95,
        edgecolors="black",
        linewidths=0.5,
        label="Точки из range query"
    )
    plt.title(f"2D-визуализация range query (ось X-Y)\nНайдено точек: {len(range_points)}")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_filename, dpi=200)
    plt.close()

    print(f"Найдено точек в диапазоне: {len(range_points)}")
    print(f"Изображение сохранено в {output_filename}")


if __name__ == "__main__":
    main()