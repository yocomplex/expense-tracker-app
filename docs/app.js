let tracker = null;

Module.onRuntimeInitialized = () => {
  tracker = new Module.ExpenseTracker();
  render();
};

function setMessage(text, isError = false) {
  const message = document.getElementById("message");
  message.textContent = text;
  message.className = isError ? "message error" : "message";
}

function addExpense() {
  try {
    const description = document.getElementById("description").value.trim();
    const amount = Number(document.getElementById("amount").value);
    const category = document.getElementById("category").value.trim();

    const id = tracker.addExpense(description, amount, category);

    document.getElementById("description").value = "";
    document.getElementById("amount").value = "";
    document.getElementById("category").value = "General";

    setMessage(`Expense added successfully. ID: ${id}`);
    render();
  } catch (error) {
    setMessage(error.message, true);
  }
}

function deleteExpense(id) {
  const deleted = tracker.deleteExpense(id);

  if (deleted) {
    setMessage("Expense deleted successfully.");
  } else {
    setMessage("Expense not found.", true);
  }

  render();
}

function render() {
  const table = document.getElementById("expenseTable");
  const emptyState = document.getElementById("emptyState");

  table.innerHTML = "";

  const expenses = tracker.listExpenses();

  if (expenses.size() === 0) {
    emptyState.style.display = "block";
  } else {
    emptyState.style.display = "none";
  }

  for (let i = 0; i < expenses.size(); i++) {
    const expense = expenses.get(i);

    const row = document.createElement("tr");

    row.innerHTML = `
      <td>${expense.id}</td>
      <td>${expense.date}</td>
      <td>${escapeHtml(expense.description)}</td>
      <td>$${expense.amount.toFixed(2)}</td>
      <td>${escapeHtml(expense.category)}</td>
      <td>
        <button class="button danger" onclick="deleteExpense(${expense.id})">
          Delete
        </button>
      </td>
    `;

    table.appendChild(row);
  }

  document.getElementById("total").textContent =
    tracker.getTotalSummary().toFixed(2);
}

function showMonthlySummary() {
  try {
    const month = Number(document.getElementById("month").value);
    const total = tracker.getMonthlySummary(month);

    document.getElementById("monthlyTotal").textContent =
      `Monthly total: $${total.toFixed(2)}`;
  } catch (error) {
    document.getElementById("monthlyTotal").textContent = error.message;
  }
}

function downloadCsv() {
  const csv = tracker.exportCsv();
  const blob = new Blob([csv], { type: "text/csv" });
  const url = URL.createObjectURL(blob);

  const link = document.createElement("a");
  link.href = url;
  link.download = "expenses.csv";
  link.click();

  URL.revokeObjectURL(url);
}

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}