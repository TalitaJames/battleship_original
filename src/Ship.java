import java.util.List;
import java.util.ArrayList;

public class Ship {

	private final List<Segment> segments;

	// ship info
	private final int length;
	private final char symbol;


	public Ship(int length, char symbol) {
		this.length = length;
		this.symbol = symbol;

		this.segments = new ArrayList<Segment>();
		for (int i = 0; i < this.length; i++) {
			this.segments.add(new Segment(this));
		}
	}
	
	public boolean sunk() {
		for (Segment s : this.segments) {
			if (!s.hit()) return false;
		}
		return true;
	}

	public Segment getSegment(int segmentNumber) {
		if (0 <= segmentNumber && segmentNumber < this.length) {
			return this.segments.get(segmentNumber);
		}
		return null;
	}

	public int getLength() {
		return length;
	}
	
	public char getSymbol() {
		return symbol;
	}
	
	@Override
	public String toString() {
		return String.valueOf(this.symbol);
	}

}
