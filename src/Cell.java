import java.io.Serializable;

public class Cell implements Serializable{ 
	private Segment segment;
	private boolean hit;

	public Cell() {
		this.segment = null;
		this.hit = false;
	}
	
	public boolean hasBeenHit() {
		return this.hit;
	}
	
	public boolean attack() {
		boolean success = false;
		if (this.segment != null){
			this.segment.attack();
			success=true;
		} 
		this.hit = true;
		return success;
	}
	
	public boolean isOccupied() {
		return this.segment != null;
	}
	
	public void placeSegment(Segment segment) {
		if (!this.isOccupied()) {
			this.segment = segment;
		}
	}
	
	@Override
	public String toString() {
		if (!this.hit) {
			return ".";
		}
		else {
			if (!this.isOccupied()) {
				return "O";
			}
			else if (!this.segment.getShip().sunk()) {
				return "X";
			}
			else {
				return this.segment.toString();
			}
		}
	}
	
	public String displaySetup() {
		return this.isOccupied() ? this.segment.toString() : ".";
	}
}
