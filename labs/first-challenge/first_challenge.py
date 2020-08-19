def get_length(arreglo):
    total = 0
    for elemento in arreglo:
        if isinstance(elemento, list):
            total += get_length(elemento)
        else:
            total += 1
    return total


print(get_length([1, [2], 1, [2], 1]))
