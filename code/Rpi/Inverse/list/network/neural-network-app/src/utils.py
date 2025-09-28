def plot_loss_curve(history):
    import matplotlib.pyplot as plt

    plt.plot(history.history['loss'], label='Training Loss')
    plt.plot(history.history['val_loss'], label='Validation Loss')
    plt.title('Loss Curve')
    plt.xlabel('Epochs')
    plt.ylabel('Loss')
    plt.legend()
    plt.show()

def save_model(model, filepath):
    model.save(filepath)

def load_model(filepath):
    from tensorflow.keras.models import load_model
    return load_model(filepath)