fn main() {
    // Look for library in dist/ locally for development/testing
    println!("cargo:rustc-link-search=native=../../dist");
    println!("cargo:rustc-link-lib=universal_timestamp");
}
