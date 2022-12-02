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

fn normalize(play: &str) -> i32 {
	if play == "X" || play == "A" {
		return 0;
	} else if play == "Y" || play == "B" {
		return 1;
	} else if play == "Z" || play == "C" {
		return 2;
	}

	-1
}

fn score(line: &str) -> i32 {
	let play_split: Split<&str> = line.split(" ");
	let plays_str: Vec<&str> = play_split.collect();
	let mut score: i32 = 0;
	let mut plays = [0, 0];

	plays[0] = normalize(&plays_str[0]);
	plays[1] = normalize(&plays_str[1]);

	if plays[0] == -1 || plays[0] == -1 {
		return -1;
	}

	if plays[1] == 0 {
		score += 1;
	} else if plays[1] == 1 {
		score += 2;
	} else if plays[1] == 2 {
		score += 3;
	}

	if plays[0] == plays[1] {
		score += 3;
	}

	if plays[1] == 0 && plays[0] == 2 {
		score += 6;
	} else if plays[1] == 1 && plays[0] == 0 {
		score += 6;
	} else if plays[1] == 2 && plays[0] == 1 {
		score += 6;
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
