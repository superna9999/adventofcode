use std::fs::File;
use std::io::{self, BufRead};
use std::path::Path;
use std::env;

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
	Ok(io::BufReader::new(File::open(filename).expect("Unable to open file")).lines())
}

fn parse_line(line: &str, trees: &mut Vec<Vec<u32>>)
{
	let mut row: Vec<u32> = Vec::new();

	for tree in line.chars() {
		row.push(tree.to_digit(10).expect("Invalid number"));
	}

	trees.push(row);
}

fn visible_trees(trees: &Vec<Vec<u32>>, col: usize, row: usize) -> u32
{
	let height = trees[row][col];
	let max_row = trees.len();
	let max_col = trees[row].len();
	let mut sum: u32;
	let mut score: u32;

	if col == 0 || row == 0 || col == (max_col - 1) || row == (max_row - 1) {
		return 0;
	}

	sum = 0;
	for c in (0..col).rev() {
		sum += 1;
		if trees[row][c] >= height {
			break;
		}
	}
	score = sum;
	
	sum = 0;
	for c in (col + 1)..max_col {
		sum += 1;
		if trees[row][c] >= height {
			break;
		}
	}
	score *= sum;
	
	sum = 0;
	for r in (0..row).rev() {
		sum += 1;
		if trees[r][col] >= height {
			break;
		}
	}
	score *= sum;

	sum = 0;
	for r in (row + 1)..max_row {
		sum += 1;
		if trees[r][col] >= height {
			break;
		}
	}
	score *= sum;
	
	score
}

fn main() {
	let args: Vec<String> = env::args().collect();
	let mut trees: Vec<Vec<u32>> = Vec::new();
	let mut scores: Vec<u32> = Vec::new();

	if args.len() < 2 {
		eprintln!("Usage: {} <input file>", args[0]);
		std::process::exit(1);
	}

	for line in read_lines(&args[1]).expect("Unable to read lines") {
		parse_line(&line.expect("Invalid line"), &mut trees);
	}

	for (r, row) in trees.iter().enumerate() {
		for (c, _col) in row.iter().enumerate() {
			scores.push(visible_trees(&trees, c, r));
		}
	}

	scores.sort();
	scores.reverse();

	println!("{:?}", scores.first().expect("invalid scores count"));
}
