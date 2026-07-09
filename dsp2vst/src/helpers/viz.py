import pandas as pd
import plotly.graph_objects as go

df = pd.read_csv("../../../intersections.csv")

fig = go.Figure()

for ray_id, group in df.groupby("ray_id"):
    fig.add_trace(go.Scatter3d(
        x=group['x'], y=group['y'], z=group['z'],
        mode='lines+markers',
        line=dict(width=2),
        marker=dict(size=2),
        name=f'Ray {ray_id}'
    ))

fig.update_layout(scene=dict(aspectmode='cube'))
fig.show()