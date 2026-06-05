from typing import Dict, List

def parse_geda_schematic(content: str) -> List[Dict]:
    """Parse gEDA schematic text format into structured data"""
    components = []
    lines = content.split('\n')
    
    for line in lines:
        if line.startswith('C '):  # Component definition
            parts = line.split()
            components.append({
                'type': 'component',
                'x': parts[1],
                'y': parts[2],
                'symbol': parts[3],
                'rotation': parts[4] if len(parts) > 4 else 0
            })
        elif line.startswith('T '):  # Text attribute
            parts = line.split(maxsplit=8)
            components.append({
                'type': 'text',
                'x': parts[1],
                'y': parts[2],
                'content': parts[8] if len(parts) > 8 else ''
            })
    
    return components
