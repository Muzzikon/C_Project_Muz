import csv
import sys
import matplotlib.pyplot as plt

def load_dbscan_csv(filename):
    points = []
    with open(filename, "r", newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            points.append({
                "x": float(row["x"]),
                "y": float(row["y"]),
                "z": float(row["z"]),
                "cluster": int(row["cluster"])
            })
    return points


def build_output_name(input_filename):
    if input_filename.lower().endswith(".csv"):
        return input_filename[:-4] + "_2d.png"
    return input_filename + "_2d.png"


def plot_dbscan(points, output_filename):
    clustered = {}
    noise_x = []
    noise_y = []

    for point in points:
        cluster_id = point["cluster"]

        if cluster_id == -1:
            noise_x.append(point["x"])
            noise_y.append(point["y"])
            continue

        if cluster_id not in clustered:
            clustered[cluster_id] = {"x": [], "y": []}

        clustered[cluster_id]["x"].append(point["x"])
        clustered[cluster_id]["y"].append(point["y"])

    plt.figure(figsize=(10, 8))

    for cluster_id in sorted(clustered.keys()):
        plt.scatter(
            clustered[cluster_id]["x"],
            clustered[cluster_id]["y"],
            s=8,
            label=f"cluster {cluster_id}"
        )

    if noise_x:
        plt.scatter(noise_x, noise_y, s=10, marker="x", label="noise")

    plt.title("DBSCAN 2D visualization (x, y)")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(output_filename, dpi=200)
    plt.close()


def main():
    if len(sys.argv) < 2:
        print("Использование: python3 scripts/plot_dbscan_2d.py <dbscan_csv>")
        return 1

    input_filename = sys.argv[1]
    output_filename = build_output_name(input_filename)

    try:
        points = load_dbscan_csv(input_filename)
    except FileNotFoundError:
        print(f"Файл не найден: {input_filename}")
        return 1
    except KeyError:
        print("Неверный формат CSV: нужны столбцы x,y,z,cluster")
        return 1
    except ValueError:
        print("Неверный формат чисел в CSV")
        return 1

    if not points:
        print("CSV-файл пустой")
        return 1

    plot_dbscan(points, output_filename)
    print(f"2D-визуализация сохранена в {output_filename}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())