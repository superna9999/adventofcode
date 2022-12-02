use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;
use std::str::Split;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	let file = File::open(filename)?;
	Ok(io::BufReader::new(file).lines())
}

fn score(line: &str) -> i32 {
	let play_split: Split<&str> = line.split(" ");
	let plays: Vec<&str> = play_split.collect();
	let mut score: i32 = 0;

	if plays[1] == "Y" {
		score += 3;
	} else if plays[1] == "Z" {
		score += 6;
	}

	if plays[1] == "X" {
		// Lose (A=>C B=>A C=>B)
		if plays[0] == "A" {
			score += 3;
		} else if plays[0] == "B" {
			score += 1;
		} else if plays[0] == "C" {
			score += 2;
		}
	} else if plays[1] == "Y" {
		// Draw (A=>A B=>B C=>C)
		if plays[0] == "A" {
			score += 1;
		} else if plays[0] == "B" {
			score += 2;
		} else if plays[0] == "C" {
			score += 3;
		}
	} else if plays[1] == "Z" {
		// Win (A=>B B=>C C=>A)
		if plays[0] == "A" {
			score += 2;
		} else if plays[0] == "B" {
			score += 3;
		} else if plays[0] == "C" {
			score += 1;
		}
	}

	return score;
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut sum: i32 = 0;

	if let Ok(lines) = read_lines(&args[1]) {
		for line in lines {
			if let Ok(ip) = line {
				sum += score(&ip);
			}
		}
	}

	println!("{}", sum);
}
