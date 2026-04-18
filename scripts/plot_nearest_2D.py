import csv
import math
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
                    float(row[2])
                except (ValueError, IndexError):
                    continue

            if len(row) < 3:
                continue

            try:
                x = float(row[0])
                y = float(row[1])
                z = float(row[2])
            except ValueError:
                continue

            points.append((x, y, z))

    return points


def parse_point(text):
    parts = [part.strip() for part in text.split(",")]

    if len(parts) != 3:
        raise ValueError("Точка должна быть в формате x,y,z")

    return float(parts[0]), float(parts[1]), float(parts[2])


def find_nearest(points, query):
    best_point = None
    best_dist_sq = float("inf")

    for point in points:
        dx = point[0] - query[0]
        dy = point[1] - query[1]
        dz = point[2] - query[2]
        dist_sq = dx * dx + dy * dy + dz * dz

        if dist_sq < best_dist_sq:
            best_dist_sq = dist_sq
            best_point = point

    return best_point, math.sqrt(best_dist_sq)


def make_output_filename(input_filename):
    os.makedirs("Results/PNG", exist_ok=True)
    base = os.path.splitext(os.path.basename(input_filename))[0]
    return os.path.join("Results", "PNG", f"{base}_nearest_2D.png")


def main():
    if len(sys.argv) != 3:
        print("Использование: python3 scripts/plot_nearest_2D.py <points_csv> <x,y,z>")
        sys.exit(1)

    input_filename = sys.argv[1]

    try:
        query = parse_point(sys.argv[2])
    except ValueError as error:
        print(f"Ошибка: {error}")
        sys.exit(1)

    points = load_points(input_filename)
    if not points:
        print("Не удалось загрузить точки из файла")
        sys.exit(1)

    nearest_point, distance = find_nearest(points, query)
    output_filename = make_output_filename(input_filename)

    all_x = [point[0] for point in points]
    all_y = [point[1] for point in points]

    plt.figure(figsize=(8, 6))
    plt.scatter(all_x, all_y, s=4, alpha=0.35, label="Все точки")
    plt.scatter(
        query[0], query[1],
        s=180,
        marker="x",
        linewidths=2.5,
        label="Точка запроса"
    )
    plt.scatter(
        nearest_point[0], nearest_point[1],
        s=180,
        marker="o",
        edgecolors="black",
        linewidths=1.2,
        label="Ближайший сосед"
    )
    plt.plot(
        [query[0], nearest_point[0]],
        [query[1], nearest_point[1]],
        linestyle="--",
        linewidth=1.2
    )

    plt.title(f"2D-визуализация nearest neighbor (ось X-Y)\nРасстояние: {distance:.6f}")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_filename, dpi=200)
    plt.close()

    print(f"Ближайшая точка: ({nearest_point[0]}, {nearest_point[1]}, {nearest_point[2]})")
    print(f"Расстояние: {distance:.6f}")
    print(f"Изображение сохранено в {output_filename}")


if __name__ == "__main__":
    main()