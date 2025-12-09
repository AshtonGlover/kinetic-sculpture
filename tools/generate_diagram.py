#!/usr/bin/env python3
"""
Generate Mermaid diagram from system architecture YAML file
"""

import yaml
import sys
from pathlib import Path


def load_yaml(file_path):
    """Load YAML file and return parsed data"""
    with open(file_path, 'r') as f:
        return yaml.safe_load(f)


def generate_mermaid_diagram(yaml_data):
    """Generate Mermaid diagram from YAML data"""
    
    diagram = ["```mermaid", "graph TB"]
    
    # Add system title and styling
    system_name = yaml_data['system']['name']
    diagram.append(f"    %% {system_name}")
    diagram.append("")
    
    # Create component nodes
    components = yaml_data['components']
    component_ids = {}
    
    diagram.append("    %% Components")
    for idx, comp in enumerate(components):
        comp_id = comp['name'].lower().replace(' ', '_').replace('-', '_')
        component_ids[comp['name']] = comp_id
        
        # Style based on type
        if comp['type'] == 'Hardware':
            shape = "([{comp_name}])"
            style = "fill:#e1f5ff,stroke:#01579b,stroke-width:2px,color:#000"
        else:
            shape = "[{comp_name}]"
            style = "fill:#fff3e0,stroke:#e65100,stroke-width:2px,color:#000"
        
        comp_name = comp['name']
        diagram.append(f"    {comp_id}{shape.format(comp_name=comp_name)}")
        diagram.append(f"    style {comp_id} {style}")
    
    diagram.append("")
    
    # Add data flow edges
    diagram.append("    %% Data Flow")
    if 'data_flow' in yaml_data:
        def map_name(name: str) -> str:
            """Map human-readable name to mermaid node id"""
            if name == "Timer ISR" or "Timer ISR" in name:
                return "timer_setup"
            if name == "Audio Buffer" or "Audio Buffer" in name:
                return "audio_processor"
            if name == "Main Loop" or "Main Loop" in name:
                return "main_loop"
            if name == "Physical Output" or "Physical Output" in name:
                return "physical_output"
            # Component match by exact or partial name
            for comp_name, comp_id in component_ids.items():
                if comp_name == name or comp_name in name:
                    return comp_id
            # Fallback slug
            return name.lower().replace(" ", "_").replace("-", "_").replace("_isr", "_setup")

        for flow in yaml_data['data_flow']:
            from_comp = map_name(flow['from'])
            to_comp = map_name(flow['to'])

            # Skip self-links (e.g., Audio Buffer folded into Audio Processor)
            if from_comp == to_comp:
                continue

            label = flow.get('data', '')
            # Note: Mermaid doesn't support <br/> or nested () in edge labels
            if 'trigger' in flow:
                # Use dash separator instead of nested parentheses
                label += f" - {flow['trigger']}"

            diagram.append(f"    {from_comp} -->|{label}| {to_comp}")
    
    diagram.append("")
    
    # Add main loop flow
    if 'main_loop' in yaml_data:
        diagram.append("    %% Main Loop Flow")
        main_loop_id = "main_loop"
        diagram.append(f"    {main_loop_id}[Main Loop]")
        diagram.append(f"    style {main_loop_id} fill:#f3e5f5,stroke:#4a148c,stroke-width:2px,color:#000")
        
        # Connect main loop to components it uses
        if 'audio_processor' in component_ids.values():
            diagram.append(f"    {main_loop_id} -->|processAudio| audio_processor")
        if 'motor_controller' in component_ids.values():
            diagram.append(f"    {main_loop_id} -->|updateMotorSpeed| motor_controller")
        if 'watchdog_timer' in component_ids.values():
            diagram.append(f"    {main_loop_id} -->|resetWatchdog| watchdog_timer")
    
    diagram.append("")
    
    # Add physical output node
    diagram.append("    %% Physical Output")
    diagram.append("    physical_output([Physical Wave Motion])")
    diagram.append("    style physical_output fill:#c8e6c9,stroke:#1b5e20,stroke-width:2px,color:#000")
    
    if 'dc_motor' in component_ids.values():
        diagram.append("    dc_motor --> physical_output")
    
    diagram.append("")
    
    # Add interrupt flow
    diagram.append("    %% Interrupt Flow")
    diagram.append("    timer_setup -.->|ISR triggers| audio_processor")
    diagram.append("    style timer_setup stroke-dasharray: 5 5")
    
    diagram.append("")
    diagram.append("```")
    
    return "\n".join(diagram)


def generate_sequence_diagram(yaml_data):
    """Generate Mermaid sequence diagram showing data flow over time"""
    
    diagram = ["```mermaid", "sequenceDiagram"]
    diagram.append("")
    
    # Get participants
    participants = []
    for comp in yaml_data['components']:
        if comp['type'] == 'Hardware' or 'Timer' in comp['name'] or 'Processor' in comp['name'] or 'Controller' in comp['name']:
            participants.append(comp['name'])
    participants.append("Main Loop")
    
    for p in participants:
        diagram.append(f"    participant {p}")
    
    diagram.append("")
    
    # Add sequence based on data flow
    if 'data_flow' in yaml_data:
        for flow in yaml_data['data_flow']:
            from_comp = flow['from']
            to_comp = flow['to']
            
            # Map to participants
            if 'Timer ISR' in from_comp:
                from_comp = "Timer Setup"
            if 'Audio Buffer' in to_comp:
                to_comp = "Audio Processor"
            if 'Main Loop' in to_comp:
                to_comp = "Main Loop"
            
            data = flow.get('data', '')
            diagram.append(f"    {from_comp}->>{to_comp}: {data}")
    
    # Add main loop interactions
    diagram.append("")
    diagram.append("    Note over Main Loop: Every iteration")
    diagram.append("    Main Loop->>Watchdog Timer: resetWatchdog()")
    diagram.append("")
    diagram.append("    Note over Main Loop: When newSampleReady")
    diagram.append("    Main Loop->>Audio Processor: processAudio()")
    diagram.append("    Main Loop->>Audio Processor: getSmoothedAmplitude()")
    diagram.append("")
    diagram.append("    Note over Main Loop: Every 10ms")
    diagram.append("    Main Loop->>Motor Controller: updateMotorSpeed(amplitude)")
    diagram.append("    Motor Controller->>DC Motor: PWM signal")
    
    diagram.append("```")
    
    return "\n".join(diagram)


def main():
    """Main function to generate diagrams"""
    script_dir = Path(__file__).parent
    yaml_file = script_dir / "system_architecture.yaml"
    
    if not yaml_file.exists():
        print(f"Error: YAML file not found at {yaml_file}")
        sys.exit(1)
    
    # Load YAML
    yaml_data = load_yaml(yaml_file)
    
    # Generate diagrams
    print("=" * 60)
    print("MERMAID FLOWCHART DIAGRAM")
    print("=" * 60)
    print()
    flowchart = generate_mermaid_diagram(yaml_data)
    print(flowchart)
    print()
    
    print("=" * 60)
    print("MERMAID SEQUENCE DIAGRAM")
    print("=" * 60)
    print()
    sequence = generate_sequence_diagram(yaml_data)
    print(sequence)
    print()
    
    # Optionally save to file
    output_file = script_dir / "diagram.md"
    with open(output_file, 'w') as f:
        f.write("# System Architecture Diagrams\n\n")
        f.write("## Flowchart Diagram\n\n")
        f.write(flowchart)
        f.write("\n\n")
        f.write("## Sequence Diagram\n\n")
        f.write(sequence)
        f.write("\n")
    
    print(f"Diagrams saved to {output_file}")


if __name__ == "__main__":
    main()

