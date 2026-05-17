const API = 'http://localhost:3000/api';
let allPersonalities = [];
let allEvents = [];
let bstData = null;
let currentFilename = '';

// D3 State
let svg, g, zoom, rootD3, treeLayout;
let i = 0;
const duration = 750;

function getCanvasWidth() {
    const container = document.getElementById('tree-container');
    return container ? container.clientWidth : 800;
}
function getCanvasHeight() {
    const container = document.getElementById('tree-container');
    return container ? container.clientHeight : 600;
}

// ════════════════════════════════════
// UPLOAD GATE
// ════════════════════════════════════
const fileInput = document.getElementById('file-input');
const btnUpload = document.getElementById('btn-upload');
const uploadDrop = document.getElementById('upload-drop');
const uploadErr = document.getElementById('upload-error');
const uploadProg = document.getElementById('upload-progress');
let selectedFile = null;

if (fileInput) {
    fileInput.addEventListener('change', () => {
        selectedFile = fileInput.files[0] || null;
        if (selectedFile) {
            uploadDrop.querySelector('.upload-text').textContent = `✓ ${selectedFile.name}`;
            uploadErr.textContent = '';
            btnUpload.disabled = false;
        }
    });

    uploadDrop.addEventListener('dragover', e => { e.preventDefault(); uploadDrop.classList.add('drag-over'); });
    uploadDrop.addEventListener('dragleave', () => uploadDrop.classList.remove('drag-over'));
    uploadDrop.addEventListener('drop', e => {
        e.preventDefault(); uploadDrop.classList.remove('drag-over');
        const file = e.dataTransfer.files[0];
        if (file) { fileInput.files = e.dataTransfer.files; fileInput.dispatchEvent(new Event('change')); }
    });

    btnUpload.addEventListener('click', async () => {
        if (!selectedFile) return;
        btnUpload.style.display = 'none';
        uploadProg.style.display = 'flex';
        uploadErr.textContent = '';

        const form = new FormData();
        form.append('db', selectedFile);

        try {
            const res = await fetch(`${API}/upload`, { method: 'POST', body: form });
            const data = await res.json();
            if (!res.ok) throw new Error(data.error || 'Upload failed');

            currentFilename = data.filename;
            document.getElementById('upload-gate').style.display = 'none';
            document.getElementById('app').style.display = 'block';
            document.getElementById('stat-file').textContent = currentFilename;
            
            initD3();
            await loadAll();
        } catch (e) {
            uploadErr.textContent = '⚠ ' + e.message;
            btnUpload.style.display = 'block';
            uploadProg.style.display = 'none';
        }
    });
}

document.getElementById('btn-change-file').addEventListener('click', showUploadGate);

function showUploadGate() {
    document.getElementById('upload-gate').style.display = 'flex';
    document.getElementById('app').style.display = 'none';
    if(btnUpload) {
        btnUpload.disabled = true;
        btnUpload.style.display = 'block';
        uploadProg.style.display = 'none';
        uploadDrop.querySelector('.upload-text').textContent = 'Click to choose a file';
    }
    selectedFile = null;
}

// ════════════════════════════════════
// NAV TABS
// ════════════════════════════════════
document.querySelectorAll('.nav-link').forEach(link => {
    link.addEventListener('click', e => {
        e.preventDefault();
        document.querySelectorAll('.nav-link').forEach(l => l.classList.remove('active'));
        link.classList.add('active');
        const target = link.dataset.tab;
        document.getElementById('section-personalities').style.display = target === 'personalities' ? 'block' : 'none';
        document.getElementById('section-events').style.display = target === 'events' ? 'block' : 'none';
    });
});

// ════════════════════════════════════
// LOAD DATA
// ════════════════════════════════════
async function loadAll() {
    await Promise.all([loadPersonalities(), loadEvents(), loadBST()]);
}

async function loadPersonalities() {
    try {
        const res = await fetch(`${API}/personalities`);
        if (!res.ok) throw new Error('Backend failed');
        allPersonalities = await res.json();
        document.getElementById('stat-count').textContent = allPersonalities.length;
    } catch (e) {
        showUploadGate();
    }
}

async function loadEvents() {
    try {
        const res = await fetch(`${API}/events`);
        if (!res.ok) throw new Error('Backend failed');
        allEvents = await res.json();
        renderTimeline(allEvents);
    } catch (e) { console.error('Events load failed', e); }
}

async function loadBST() {
    try {
        const res = await fetch(`${API}/bst`);
        if (!res.ok) throw new Error('Backend failed');
        bstData = await res.json();
        document.getElementById('stat-bst-height').textContent = getDepth(bstData);
        updateD3(bstData);
    } catch (e) { console.error('BST load failed', e); }
}

function getDepth(node) {
    if (!node) return 0;
    return 1 + Math.max(getDepth(node.left), getDepth(node.right));
}

// ════════════════════════════════════
// D3 TREE VISUALIZATION
// ════════════════════════════════════
function initD3() {
    const container = d3.select("#tree-container");
    container.selectAll("*").remove(); // Clear

    zoom = d3.zoom()
        .scaleExtent([0.1, 3])
        .on("zoom", (event) => g.attr("transform", event.transform));

    svg = container.append("svg")
        .attr("width", "100%")
        .attr("height", "100%")
        .call(zoom)
        .on("dblclick.zoom", null); // Disable double click zoom

    const w = getCanvasWidth();
    g = svg.append("g")
        .attr("transform", `translate(${w / 2}, 60)`);

    treeLayout = d3.tree().nodeSize([90, 120]);
}

function updateD3(sourceData) {
    if (!sourceData) return;

    // Convert custom BST JSON to D3 hierarchy format
    function buildHierarchy(node) {
        if (!node) return null;
        let children = [];
        
        if (node.left || node.right) {
            if (node.left) {
                children.push(buildHierarchy(node.left));
            } else {
                children.push({ name: 'dummy_' + Math.random(), isDummy: true, children: null });
            }
            if (node.right) {
                children.push(buildHierarchy(node.right));
            } else {
                children.push({ name: 'dummy_' + Math.random(), isDummy: true, children: null });
            }
        }
        
        return { name: node.name, isDummy: false, children: children.length > 0 ? children : null };
    }

    const d3Data = buildHierarchy(sourceData);
    const newRoot = d3.hierarchy(d3Data);
    
    // Preserve old coordinates for smooth animation
    if (rootD3) {
        const oldNodes = new Map(rootD3.descendants().map(d => [d.data.name, d]));
        newRoot.each(d => {
            const old = oldNodes.get(d.data.name);
            if (old) {
                d.x0 = old.x0;
                d.y0 = old.y0;
            } else if (d.parent && oldNodes.has(d.parent.data.name)) {
                // If it's a new node, start it from its parent's old position
                const oldParent = oldNodes.get(d.parent.data.name);
                d.x0 = oldParent.x0;
                d.y0 = oldParent.y0;
            } else {
                d.x0 = getCanvasWidth() / 2;
                d.y0 = 60;
            }
        });
    } else {
        newRoot.x0 = getCanvasWidth() / 2;
        newRoot.y0 = 60;
    }

    rootD3 = newRoot;
    renderD3(rootD3);
}

function renderD3(source) {
    const treeData = treeLayout(rootD3);
    const nodes = treeData.descendants();
    const links = treeData.descendants().slice(1);

    // Normalize for fixed-depth
    nodes.forEach(d => { d.y = d.depth * 100; });

    // --- NODES ---
    const node = g.selectAll('g.node')
        .data(nodes, d => d.data.name); // Key by name for animation!

    const nodeEnter = node.enter().append('g')
        .attr('class', 'node')
        .attr("transform", d => `translate(${source.x0},${source.y0})`)
        .on('click', (event, d) => {
            if (!d.data.isDummy) handleNodeClick(d.data.name);
        });

    nodeEnter.append('circle')
        .attr('r', 1e-6);

    nodeEnter.append('text')
        .attr("dy", 28)
        .attr("text-anchor", "middle")
        .text(d => d.data.isDummy ? '' : (d.data.name.length > 12 ? d.data.name.substring(0, 11) + '...' : d.data.name))
        .style('fill-opacity', 1e-6);

    const nodeUpdate = nodeEnter.merge(node);

    nodeUpdate.transition()
        .duration(duration)
        .attr("transform", d => `translate(${d.x},${d.y})`);

    nodeUpdate.select('circle')
        .attr('r', d => d.data.isDummy ? 0 : 16)
        .attr('cursor', d => d.data.isDummy ? 'default' : 'pointer');

    nodeUpdate.select('text')
        .style('fill-opacity', d => d.data.isDummy ? 0 : 1);

    const nodeExit = node.exit().transition()
        .duration(duration)
        .attr("transform", d => `translate(${source.x},${source.y})`)
        .remove();

    nodeExit.select('circle').attr('r', 1e-6);
    nodeExit.select('text').style('fill-opacity', 1e-6);

    // --- LINKS ---
    const link = g.selectAll('path.link')
        .data(links, d => d.data.name); // Key by target node name

    const linkEnter = link.enter().insert('path', "g")
        .attr("class", "link")
        .attr('d', d => {
            const o = {x: source.x0, y: source.y0};
            return diagonal(o, o);
        });

    const linkUpdate = linkEnter.merge(link);

    linkUpdate.transition()
        .duration(duration)
        .attr('d', d => diagonal(d, d.parent))
        .style('stroke-opacity', d => d.data.isDummy ? 0 : 1);

    link.exit().transition()
        .duration(duration)
        .attr('d', d => {
            const o = {x: source.x, y: source.y};
            return diagonal(o, o);
        })
        .remove();

    nodes.forEach(d => {
        d.x0 = d.x;
        d.y0 = d.y;
    });

    function diagonal(s, d) {
        return `M ${s.x} ${s.y} C ${s.x} ${(s.y + d.y) / 2}, ${d.x} ${(s.y + d.y) / 2}, ${d.x} ${d.y}`;
    }
}

// ════════════════════════════════════
// SEARCH & PANNING
// ════════════════════════════════════
document.getElementById('search-input').addEventListener('input', function () {
    const q = this.value.toLowerCase().trim();
    g.selectAll('.node').classed('highlight', false);

    if (!q) return;

    let targetNode = null;
    rootD3.each(d => {
        if (d.data.name.toLowerCase().includes(q)) {
            if (!targetNode) targetNode = d; // pan to first match
            // Highlight matching nodes
            g.selectAll('.node').filter(n => n.data.name === d.data.name).classed('highlight', true);
        }
    });

    if (targetNode) {
        const w = getCanvasWidth();
        const h = getCanvasHeight();
        const x = -targetNode.x + w / 2;
        const y = -targetNode.y + h / 2;
        svg.transition().duration(750).call(
            zoom.transform,
            d3.zoomIdentity.translate(x, y).scale(1)
        );
    }
});

document.getElementById('btn-center').addEventListener('click', () => {
    const w = getCanvasWidth();
    svg.transition().duration(750).call(
        zoom.transform,
        d3.zoomIdentity.translate(w / 2, 60).scale(1)
    );
});

// ════════════════════════════════════
// ADD PERSONALITY (INSERT)
// ════════════════════════════════════
document.getElementById('btn-show-add').addEventListener('click', () => {
    document.getElementById('modal-add-overlay').style.display = 'flex';
});

document.getElementById('modal-add-close').addEventListener('click', () => {
    document.getElementById('modal-add-overlay').style.display = 'none';
});

function insertIntoBST(node, newNode) {
    if (!node) return newNode;
    const newName = newNode.name.toLowerCase();
    const nodeName = node.name.toLowerCase();
    
    if (newName < nodeName) {
        if (!node.left) node.left = newNode;
        else insertIntoBST(node.left, newNode);
    } else if (newName > nodeName) {
        if (!node.right) node.right = newNode;
        else insertIntoBST(node.right, newNode);
    }
    return node;
}

document.getElementById('btn-submit-add').addEventListener('click', async () => {
    const name = document.getElementById('add-name').value.trim();
    const dob = document.getElementById('add-dob').value.trim();
    const dod = document.getElementById('add-dod').value.trim();
    const def = document.getElementById('add-def').value.trim();
    const err = document.getElementById('add-error');

    if (!name || !dob) { err.textContent = 'Name and Date of Birth are required.'; return; }
    err.textContent = '';
    document.getElementById('btn-submit-add').textContent = 'Inserting...';

    try {
        const res = await fetch(`${API}/personality`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, dob, dod, definition: def })
        });
        const data = await res.json();
        if (!res.ok) throw new Error(data.error || 'Failed to add');

        // Close modal & reset form
        document.getElementById('modal-add-overlay').style.display = 'none';
        document.getElementById('add-name').value = '';
        document.getElementById('add-dob').value = '';
        document.getElementById('add-dod').value = '';
        document.getElementById('add-def').value = '';
        
        // Custom Javascript insert at LEAF instead of reloading from C
        const newNode = { name, dob, dod, definition: def, left: null, right: null };
        allPersonalities.push(newNode);
        
        if (!bstData) {
            bstData = newNode;
        } else {
            insertIntoBST(bstData, newNode);
        }
        
        // Update Stats and D3
        document.getElementById('stat-count').textContent = allPersonalities.length;
        document.getElementById('stat-bst-height').textContent = getDepth(bstData);
        updateD3(bstData);
        
        // Search and pan to new node
        document.getElementById('search-input').value = name;
        document.getElementById('search-input').dispatchEvent(new Event('input'));
        
    } catch (e) {
        err.textContent = e.message;
    } finally {
        document.getElementById('btn-submit-add').textContent = 'Insert into Tree';
    }
});

// ════════════════════════════════════
// MODALS (DETAILS & DELETE)
// ════════════════════════════════════
let currentSelectedNode = null;
let currentSelectedEvent = null;

function handleNodeClick(name) {
    const p = allPersonalities.find(x => x.name === name);
    if (p) openModal(p);
}

function openModal(p) {
    currentSelectedNode = p.name;
    currentSelectedEvent = null;
    document.getElementById('modal-badge').textContent = 'Historical Figure';
    document.getElementById('modal-name').textContent = p.name;
    const datesEl = document.getElementById('modal-dates');
    datesEl.innerHTML = '';
    if (p.dob) datesEl.innerHTML += `<span class="badge badge-dob">Born ${escHtml(p.dob)}</span>`;
    if (p.dod) datesEl.innerHTML += `<span class="badge badge-dod">Died ${escHtml(p.dod)}</span>`;
    document.getElementById('modal-def').textContent = p.definition || 'No description available in the database.';
    document.getElementById('modal-overlay').style.display = 'flex';
}

document.getElementById('btn-delete-node').addEventListener('click', async () => {
    if (!currentSelectedNode && !currentSelectedEvent) return;
    
    const targetName = currentSelectedNode || currentSelectedEvent;
    const type = currentSelectedNode ? 'personality' : 'event';
    
    if (!confirm(`Are you sure you want to permanently delete ${targetName}?`)) return;

    try {
        const res = await fetch(`${API}/${type}/${encodeURIComponent(targetName)}`, { method: 'DELETE' });
        if (!res.ok) throw new Error(`Failed to delete ${type}`);
        
        closeModal();
        document.getElementById('search-input').value = '';
        
        if (type === 'personality') {
            await loadAll(); // Reload tree
        } else {
            await loadEvents(); // Reload timeline
        }
        
    } catch (e) {
        alert(e.message);
    }
});

function openEventModal(ev) {
    currentSelectedEvent = ev.name;
    currentSelectedNode = null;
    document.getElementById('modal-badge').textContent = 'Historical Event';
    document.getElementById('modal-name').textContent = ev.name;
    const datesEl = document.getElementById('modal-dates');
    datesEl.innerHTML = `<span class="badge badge-dob">Date: ${escHtml(ev.date)}</span>`;
    document.getElementById('modal-def').textContent = ev.description || 'No description available.';
    document.getElementById('modal-overlay').style.display = 'flex';
}

document.getElementById('modal-close').addEventListener('click', closeModal);
document.getElementById('modal-overlay').addEventListener('click', e => { if (e.target.id === 'modal-overlay') closeModal(); });
document.addEventListener('keydown', e => { if (e.key === 'Escape') closeModal(); });
function closeModal() { document.getElementById('modal-overlay').style.display = 'none'; }

// ════════════════════════════════════
// RENDER TIMELINE (Events)
// ════════════════════════════════════
function renderTimeline(events) {
    const sorted = [...events].sort((a, b) => a.date.localeCompare(b.date));
    const wrap = document.getElementById('timeline-wrap');
    if (!sorted.length) { wrap.innerHTML = '<div class="loading-spinner"><p>No events available.</p></div>'; return; }
    wrap.innerHTML = sorted.map((ev, i) => `
        <div class="timeline-item timeline-hidden" data-idx="${i}">
            <div class="timeline-card">
                <div class="timeline-dot"></div>
                <div class="timeline-year">${escHtml(ev.date)}</div>
                <div class="timeline-name">${escHtml(ev.name)}</div>
                <div class="timeline-def">${escHtml(ev.description || '—')}</div>
            </div>
        </div>
    `).join('');
    
    wrap.querySelectorAll('.timeline-item').forEach(item => {
        item.addEventListener('click', () => {
            const ev = sorted[parseInt(item.dataset.idx)];
            if (ev) openEventModal(ev);
        });
    });

    // Intersection Observer for scroll animation
    const observer = new IntersectionObserver(entries => {
        entries.forEach(entry => {
            if (entry.isIntersecting) {
                entry.target.classList.add('timeline-visible');
                observer.unobserve(entry.target);
            }
        });
    }, { threshold: 0.1, rootMargin: '0px 0px -50px 0px' });

    wrap.querySelectorAll('.timeline-hidden').forEach(el => observer.observe(el));
}

function escHtml(str) {
    return (str || '').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}
