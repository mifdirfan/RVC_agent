# Data Dictionary

## 1. External Input/Output Events

| Input/Output Event | Description | Format / Type |
|---|---|---|
| Front Sensor Input | Detects obstacles in front of the RVC. | Boolean |
| Left Sensor Input | Detects obstacles on the left side. | Boolean |
| Right Sensor Input | Detects obstacles on the right side. | Boolean |
| Dust Sensor Input | Detects dust on the floor. | Boolean |
| Direction | Movement direction command. | Enum |
| Clean | Cleaning mode command. | Enum |

## 2. Internal Data and Control Flows

| Data Name | Description | Format / Type | Flow Category | Source | Destination | DFD Level |
|---|---|---|---|---|---|---|
| Front Obstacle | Front obstacle detection result | Boolean | Data | Process 1.1 | Process 1.5 | 2 |
| Left Obstacle | Left obstacle detection result | Boolean | Data | Process 1.2 | Process 1.5 | 2 |
| Right Obstacle | Right obstacle detection result | Boolean | Data | Process 1.3 | Process 1.5 | 2 |
| Tick | Clock event | Event | Control | Digital Clock | Controller | 0-3 |

## 3. Data Store Definitions

| Data Store | Description | Stored Data | Format / Type | DFD Level |
|---|---|---|---|---|
| Obstacle Location | Stores detected obstacle locations | Front, Left, Right | TBD | 1-3 |
| Dust Existence | Stores dust detection result | Dust | Boolean | 1-3 |

## 4. Detailed Data Definitions

For each data item, define:

### Data Name
- Description:
- Format / Type:
- Possible Values:
- Source:
- Destination:
- DFD Level:
- Requirement Reference:

## Formatting Rules
- Use Markdown tables.
- Preserve event names from the Event List.
- Define all data and control flows.
- Include newly introduced internal flows.
- Include all Data Stores.
- Use consistent data names across levels.
- Use TBD when a data type is not specified.
- Do not invent unsupported values or formats.
- Treat example entries as illustrative only.
