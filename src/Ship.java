import java.util.List;
import java.util.ArrayList;
import java.io.Serializable;

public class Ship  implements Serializable, Comparable<Ship>{
	private final List<ShipSegment> shipSegments;

	private final int length;
	private final char symbol;


	public Ship(int length, char symbol) {
		this.length = length;
		this.symbol = symbol;

		this.shipSegments = new ArrayList<ShipSegment>();
		for (int i = 0; i < this.length; i++) {
			this.shipSegments.add(new ShipSegment(this));
		}
	}
	
	public boolean sunk() {
		for (ShipSegment s : this.shipSegments) {
			if (!s.hit()) return false;
		}
		return true;
	}

	public ShipSegment getShipSegment(int shipSegmentNumber) {
		if (0 <= shipSegmentNumber && shipSegmentNumber < this.length) {
			return this.shipSegments.get(shipSegmentNumber);
		}
		return null;
	}

	public int getLength() {
		return length;
	}
	
	public char getSymbol() {
		return symbol;
	}
	
	public int compareTo(Ship newShip){
		return this.getLength() - newShip.getLength();
	}

	@Override
	public String toString() {
		return String.valueOf(this.symbol);
	}

}
