# Overview
This repository consists of two critical sections: the core and the wiki. Together, they establish the foundation for all Moztopia projects. Before proceeding, update your project’s `.gitignore` file to include:

```.gitignore
.wiki
.github
```

Note: You are cloning repositories *into* your existing project. If changes are made to the `main` branches of these source repositories, you’ll need to pull those updates manually. These repositories are foundational only—they do not contain project-specific structure or logic.

## .github

From your project root, execute:

```bash
git clone https://github.com/moztopia/github .github
```

This repository provides **GitHub-specific configuration** files required for all Moztopia projects.

## .wiki

This setup is slightly more involved. Follow these steps to initialize the wiki:

```bash
# Step 1: Clone the global wiki template into the .wiki directory
git clone https://github.com/moztopia/github.wiki .wiki

# Step 2: Remove the existing Git history from the template
rm -rf .wiki/.git

# Step 3: Reinitialize a new Git repository for the target wiki
cd .wiki
git init
git remote add origin https://github.com/moztopia/REPO_NAME.wiki.git

# Step 4: Stage and commit the template content
git add -A
git commit -m "Initialize Standard Moztopia Project Wiki"

# Step 5: Set the default branch to match GitHub Wiki's expectations
git branch -M master

# Step 6: Push the wiki content to the GitHub-hosted wiki repository
git push --force origin master
```

---
